#include "IO.h"
#include <nltools/threading/Threading.h>
#include <nltools/messaging/Message.h>
#include <nltools/logging/Log.h>
#include <nltools/ExceptionTools.h>

IO::IO(snd_rawmidi_t *handle)
    : m_handle(handle)
{
}

IO::IO(IO &&i)
    : m_handle(std::exchange(i.m_handle, nullptr))
{
}

IO::~IO()
{
  if(m_handle)
    snd_rawmidi_close(m_handle);
}

Input::Input(Input &&i)
    : IO(std::move(i))
    , m_bg(std::move(i.m_bg))
{
}

Input::Input(const std::string &name)
    : IO(open(name))
{
  pipe(m_cancelPipe);
  m_bg = std::async(std::launch::async, [=] { readMidi(); });
}

Input::~Input()
{
  m_quit = true;
  uint8_t v = 0;
  write(m_cancelPipe[1], &v, 1);
  m_bg.wait();
}

struct MidiEventCodecs
{
  MidiEventCodecs(unsigned encoderSize, unsigned decoderSize)
  {
    snd_midi_event_new(encoderSize, &encoder);
    snd_midi_event_new(decoderSize, &decoder);

    if(encoder == nullptr || decoder == nullptr)
      nltools::Log::throwException("Could not set up midi parser buffers");

    snd_midi_event_no_status(decoder, 1);  // force full-qualified midi events (no "running status")
  }

  ~MidiEventCodecs()
  {
    if(encoder)
      snd_midi_event_free(encoder);

    if(decoder)
      snd_midi_event_free(decoder);
  }

  snd_midi_event_t *encoder = nullptr;
  snd_midi_event_t *decoder = nullptr;
};

void Input::readMidi()
{
  using namespace nltools::msg;

  nltools::threading::setThisThreadPrio(nltools::threading::Priority::AlmostRealtime);

  // Raw bytes buffer for snd_raw_midi_read().
  // For large SysEx data coming in fast a fair amount of background buffer is really required
  constexpr unsigned RAWMIDI_BUF_SIZE = 131072;  // empirical value + safety margin

  // Maximum granularity of the main receive loop.
  // Typically, incoming data between polls will be in much smaller sized blocks.
  constexpr unsigned LOCAL_BUF_SIZE = 1024;  // non-critical value

  uint8_t buffer[LOCAL_BUF_SIZE];
  size_t bufferReserve = RAWMIDI_BUF_SIZE;

  int numPollFDs = snd_rawmidi_poll_descriptors_count(m_handle);
  pollfd pollFileDescriptors[numPollFDs + 1];
  numPollFDs = snd_rawmidi_poll_descriptors(m_handle, pollFileDescriptors, numPollFDs);
  pollFileDescriptors[numPollFDs].fd = m_cancelPipe[0];
  pollFileDescriptors[numPollFDs].events = POLLIN;

  if(!setupDevice(RAWMIDI_BUF_SIZE))
    nltools::Log::throwException("Could not set up midi receive buffer");

  snd_rawmidi_status_t *pStatus;
  snd_rawmidi_status_alloca(&pStatus);

  MidiEventCodecs codecs(LOCAL_BUF_SIZE, 128);

  while(!m_quit)
  {
    if(auto result = snd_rawmidi_status(m_handle, pStatus))
      nltools::Log::throwException("Could read data/status from midi input file descriptor =>", snd_strerror(result));

    if(snd_rawmidi_status_get_xruns(pStatus) > 0)
    {
      nltools::Log::warning("raw_midi receive buffer overrun");
      snd_midi_event_reset_decode(codecs.decoder);  // purge any half-decoded events
    }

    bufferReserve = logReserve(pStatus, RAWMIDI_BUF_SIZE, bufferReserve);

    if(auto result = fetchChunk(buffer, LOCAL_BUF_SIZE, numPollFDs, pollFileDescriptors))
      processChunk(buffer, result, codecs.encoder, codecs.decoder);
  }
}

size_t Input::logReserve(snd_rawmidi_status_t *pStatus, size_t bufferSize, size_t oldReserve) const
{
  auto currentBufferReserve = bufferSize - snd_rawmidi_status_get_avail(pStatus);
  if(currentBufferReserve < oldReserve)
  {
    nltools::Log::info("raw_midi receive buffer headroom : ", currentBufferReserve);
    return currentBufferReserve;
  }
  return oldReserve;
}

size_t Input::fetchChunk(uint8_t *buffer, size_t length, int numPollFDs, pollfd *pollFileDescriptors)
{
  auto result = snd_rawmidi_read(m_handle, buffer, length);
  if(result == -EAGAIN)  // nothing remaining or no data
  {
    // now sleep until data is coming in ...
    if(poll(pollFileDescriptors, numPollFDs + 1, -1) > 0)
      return 0;

    nltools::Log::throwException("Polling the midi input file descriptor failed.");
  }

  if(result < 0)  // catch errors
    nltools::Log::throwException("Error reading midi: ", snd_strerror(result));

  return result;
}

void Input::processChunk(uint8_t *buffer, size_t length, snd_midi_event_t *encoder, snd_midi_event_t *decoder)
{
  using namespace nltools::msg;

  snd_seq_event_t event;

  while(length > 0)
  {
    auto consumed = snd_midi_event_encode(encoder, buffer, length, &event);

    if(consumed <= 0)
    {
      nltools::Log::error("Could not encode stream into midi event =>", snd_strerror(consumed));
      snd_midi_event_reset_encode(encoder);
      return;
    }

    length -= consumed;
    buffer += consumed;

    // TODO : Bundle available messages into one packet for less IP overhead ?
    if(event.type < SND_SEQ_EVENT_SONGPOS)
    {
      // event is complete *and* is relevant for us --> reconvert event to 1..3 bytes raw data
      snd_midi_event_reset_decode(decoder);
      Midi::SimpleMessage msg;
      msg.numBytesUsed = std::min(3l, snd_midi_event_decode(decoder, msg.rawBytes.data(), 3, &event));
      send(EndPoint::ExternalMidiOverIPClient, msg);
    }
  }
}

bool Input::setupDevice(size_t bufferSize)
{
  snd_rawmidi_nonblock(m_handle, 1);  // make reads non-blocking

  snd_rawmidi_params_t *pParams;
  snd_rawmidi_params_alloca(&pParams);
  snd_rawmidi_params_current(m_handle, pParams);
  snd_rawmidi_params_set_buffer_size(m_handle, pParams, bufferSize);
  snd_rawmidi_params(m_handle, pParams);
  snd_rawmidi_params_current(m_handle, pParams);

  return snd_rawmidi_params_get_buffer_size(pParams) == bufferSize;
}

snd_rawmidi_t *Input::open(const std::string &name)
{
  snd_rawmidi_t *h = nullptr;
  snd_rawmidi_open(&h, nullptr, name.c_str(), SND_RAWMIDI_NONBLOCK);
  return h;
}

Output::Output(Output &&i)
    : IO(std::move(i))
{
}

Output::Output(const std::string &name)
    : IO(open(name))
{
}

void Output::send(const nltools::msg::Midi::SimpleMessage &msg)
{
  if(auto res = snd_rawmidi_write(m_handle, msg.rawBytes.data(), msg.numBytesUsed))
  {
    if(size_t(res) != msg.numBytesUsed)
      nltools::Log::error("Could not write message into midi output device");
    else
      snd_rawmidi_drain(m_handle);
  }
}

snd_rawmidi_t *Output::open(const std::string &name)
{
  snd_rawmidi_t *h = nullptr;
  snd_rawmidi_open(nullptr, &h, name.c_str(), SND_RAWMIDI_NONBLOCK);
  return h;
}
