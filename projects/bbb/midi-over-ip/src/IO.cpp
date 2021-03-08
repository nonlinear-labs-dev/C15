#include "IO.h"
#include <nltools/threading/Threading.h>
#include <nltools/messaging/Message.h>
#include <nltools/logging/Log.h>

IO::IO(snd_rawmidi_t *handle)
    : handle(handle)
{
}

IO::IO(IO &&i)
    : handle(std::exchange(i.handle, nullptr))
{
}

IO::~IO()
{
  if(handle)
    snd_rawmidi_close(handle);
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

void Input::readMidi()
{
// check raw_midi buffer overruns
#define CHECK_XRUNS (1)

  using namespace nltools::msg;

  nltools::threading::setThisThreadPrio(nltools::threading::Priority::AlmostRealtime);

  // Raw bytes buffer for snd_raw_midi_read()
  constexpr unsigned RAWMIDI_BUF_SIZE = 524288;  // 512k, puh... and it still overruns from time to time

  // The larger this buffer the more events MIDI events can be extracted in one go from a burst of data.
  constexpr unsigned LOCAL_BUF_SIZE = 4096;

  constexpr unsigned ENCODER_SIZE = 4096;
  constexpr unsigned DECODER_SIZE = 128;

  snd_rawmidi_params_t *pParams;
  snd_rawmidi_params_alloca(&pParams);
  auto err = snd_rawmidi_params_set_buffer_size(handle, pParams, RAWMIDI_BUF_SIZE);
  if(err < 0)
  {
    nltools::Log::error("Could not set midi buffer size");
    return;
  }

  uint8_t *buffer = new uint8_t[LOCAL_BUF_SIZE];

  snd_seq_event_t event;

  // Parsers to encode/decode byte stream <--> midi event
  // These parsers run asynchronous to the main buffer data chunks --> must be global
  snd_midi_event_t *encoder = nullptr;
  snd_midi_event_t *decoder = nullptr;
  snd_midi_event_new(ENCODER_SIZE, &encoder);  // temporary buffer for encoding into midi events
  snd_midi_event_new(DECODER_SIZE, &decoder);  // re-decoded midi bytes per event to transmit to app (3, currently)

  snd_midi_event_no_status(decoder, 1);  // force full-qualified midi events (no "running status")
  snd_rawmidi_nonblock(handle, 1);       // make reads non-blocking

#if CHECK_XRUNS
  snd_rawmidi_status_t *pStatus;
  snd_rawmidi_status_alloca(&pStatus);
#endif

  int numPollFDs = snd_rawmidi_poll_descriptors_count(handle);
  pollfd pollFileDescriptors[numPollFDs + 1];
  numPollFDs = snd_rawmidi_poll_descriptors(handle, pollFileDescriptors, numPollFDs);
  pollFileDescriptors[numPollFDs].fd = m_cancelPipe[0];
  pollFileDescriptors[numPollFDs].events = POLLIN;

  while(!m_quit)
  {
    auto readResult = snd_rawmidi_read(handle, buffer, LOCAL_BUF_SIZE);  // try read a block of bytes, non-blocking

    if(readResult == -EAGAIN)  // nothing remaining or no data
    {
      auto pollResult = poll(pollFileDescriptors, numPollFDs + 1, -1);  // sleep until data is coming in
      if(pollResult == POLLPRI || pollResult == POLLRDHUP || pollResult == POLLERR || pollResult == POLLHUP
         || pollResult == POLLNVAL)
      {
        nltools::Log::error("Polling the midi input file descriptor failed. Terminating.");
        break;
      }
      continue;  // reading
    }

    if(readResult < 0)
    {
      nltools::Log::error("Could not read from midi input file descriptor =>", snd_strerror(readResult));
      break;  // quit (unplugging event, typically)
    }

    auto remaining = readResult;  // # of bytes
    auto currentBuffer = buffer;  // position in buffer

    while(remaining > 0)  // process a chunk of bytes
    {
      auto consumed = snd_midi_event_encode(encoder, currentBuffer, remaining, &event);  // encode next
      if(consumed <= 0)
      {
        nltools::Log::error("Could not encode stream into midi event =>", snd_strerror(consumed));
        snd_midi_event_reset_encode(encoder);
        break;  // discard and continue reading
      }

      remaining -= consumed;
      currentBuffer += consumed;

      // TODO : Bundle available messages into one packet
      if(event.type < SND_SEQ_EVENT_SONGPOS)  // event complete and relevant for us
      {
        Midi::SimpleMessage msg;
        snd_midi_event_reset_decode(decoder);
        // reconvert to bytes
        msg.numBytesUsed = std::min(3l, snd_midi_event_decode(decoder, msg.rawBytes.data(), 3, &event));
        send(EndPoint::ExternalMidiOverIPClient, msg);
      }
    }

#if CHECK_XRUNS
    auto err = snd_rawmidi_status(handle, pStatus);
    if(err)
    {
      nltools::Log::error("Could not get midi status =>", snd_strerror(err));
      break;
    }
    err = snd_rawmidi_status_get_xruns(pStatus);
    if(err > 0)
    {
      nltools::Log::error("rawmidi receive buffer overrun");
      snd_midi_event_reset_decode(decoder);
      continue;  // discard buffer contents and continue reading
    }
#endif
  }  // while(!m_quit)

  snd_midi_event_free(encoder);
  snd_midi_event_free(decoder);
  delete[] buffer;
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
  if(auto res = snd_rawmidi_write(handle, msg.rawBytes.data(), msg.numBytesUsed))
  {
    if(size_t(res) != msg.numBytesUsed)
      nltools::Log::error("Could not write message into midi output device");
    else
      snd_rawmidi_drain(handle);
  }
}

snd_rawmidi_t *Output::open(const std::string &name)
{
  snd_rawmidi_t *h = nullptr;
  snd_rawmidi_open(nullptr, &h, name.c_str(), SND_RAWMIDI_NONBLOCK);
  return h;
}
