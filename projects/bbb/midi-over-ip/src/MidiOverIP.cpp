#include "Options.h"
#include <nltools/messaging/Message.h>
#include <nltools/messaging/Messaging.h>
#include <nltools/threading/Threading.h>
#include <alsa/asoundlib.h>
#include <stdlib.h>
#include <thread>
#include <array>
#include <glibmm.h>

static bool quitApp = false;
static bool enableMidi = false;
static Glib::RefPtr<Glib::MainLoop> loop;
static int cancelPipe[2];

void quit(int)
{
  quitApp = true;
  uint8_t v = 0;
  write(cancelPipe[1], &v, 1);
  loop->quit();
}

void readMidi(int cancelHandle, snd_rawmidi_t *inputHandle)
{
  using namespace nltools::msg;

  nltools::threading::setThisThreadPrio(nltools::threading::Priority::AlmostRealtime);

  while(!quitApp)
  {
    uint8_t byte = 0;
    snd_midi_event_t *encoder = nullptr;
    snd_midi_event_new(128, &encoder);

    snd_midi_event_t *decoder = nullptr;
    snd_midi_event_new(128, &decoder);
    snd_midi_event_no_status(decoder, 1);

    int numPollFDs = snd_rawmidi_poll_descriptors_count(inputHandle);

    pollfd pollFileDescriptors[numPollFDs + 1];
    numPollFDs = snd_rawmidi_poll_descriptors(inputHandle, pollFileDescriptors, numPollFDs);
    pollFileDescriptors[numPollFDs].fd = cancelHandle;
    pollFileDescriptors[numPollFDs].events = POLLIN;

    while(true)
    {
      switch(poll(pollFileDescriptors, numPollFDs + 1, -1))
      {
        case POLLPRI:
        case POLLRDHUP:
        case POLLERR:
        case POLLHUP:
        case POLLNVAL:
          nltools::Log::error("Polling the midi input file descriptor failed. Terminating.");
          quitApp = true;
          break;

        default:
          break;
      }

      if(quitApp)
        break;

      snd_seq_event_t event;

      while(!quitApp)
      {
        auto readResult = snd_rawmidi_read(inputHandle, &byte, 1);

        if(readResult == 1)
        {
          if(snd_midi_event_encode_byte(encoder, byte, &event) == 1)
          {
            if(event.type != SND_SEQ_EVENT_NONE)
            {
              if(enableMidi)
              {
                Midi::SimpleMessage msg;
                msg.numBytesUsed = snd_midi_event_decode(decoder, msg.rawBytes.data(), 3, &event);
                send(EndPoint::ExternalMidiOverIPClient, msg);
              }
              break;
            }
          }
        }
        else if(readResult == -19)
        {
          nltools::Log::error("Could not read from midi input file descriptor =>", snd_strerror(readResult));
          quit(0);
        }
      }
    }
  }
}

void configureMessaging(const Options &options, bool hasInput, bool hasOutput)
{
  using namespace nltools::msg;
  using nltools::threading::Priority;

  auto aeHost = options.getAudioEngineHost();

  Configuration conf;

  conf.offerEndpoints = { { EndPoint::ExternalMidiOverIPBridge, Priority::AlmostRealtime } };

  if(hasInput)
    conf.useEndpoints = { { EndPoint::ExternalMidiOverIPClient, aeHost, Priority::AlmostRealtime } };

  nltools::msg::init(conf);
}

void sendToExternalDevice(snd_rawmidi_t *outputHandle, const nltools::msg::Midi::SimpleMessage &msg)
{
  if(!enableMidi)
    return;

  if(auto res = snd_rawmidi_write(outputHandle, msg.rawBytes.data(), msg.numBytesUsed))
    if(size_t(res) != msg.numBytesUsed)
      nltools::Log::error("Could not write message into midi output device");

  snd_rawmidi_drain(outputHandle);
}

void runMainLoop()
{
  loop = Glib::MainLoop::create();
  loop->run();
}

int openMidiDevices(const Options &options, snd_rawmidi_t *&inputHandle, snd_rawmidi_t *&outputHandle)
{
  if(!options.getMidiInDevice().empty())
  {
    if(snd_rawmidi_open(&inputHandle, nullptr, options.getMidiInDevice().c_str(), SND_RAWMIDI_NONBLOCK))
    {
      nltools::Log::error("Could not open midi in device");
      return EXIT_FAILURE;
    }
  }

  if(!options.getMidiOutDevice().empty())
  {
    if(snd_rawmidi_open(nullptr, &outputHandle, options.getMidiOutDevice().c_str(), SND_RAWMIDI_NONBLOCK))
    {
      nltools::Log::error("Could not open midi out device");
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int main(int args, char *argv[])
{
  using namespace nltools::msg;
  using namespace nltools::msg::Midi;
  using namespace std::chrono;

  signal(SIGINT, quit);
  signal(SIGQUIT, quit);
  signal(SIGTERM, quit);

  Options options(args, argv);

  snd_rawmidi_t *inputHandle = nullptr;
  snd_rawmidi_t *outputHandle = nullptr;

  if(openMidiDevices(options, inputHandle, outputHandle) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  configureMessaging(options, inputHandle, outputHandle);

  constexpr auto endPoint = EndPoint::ExternalMidiOverIPBridge;

  std::thread sender;

  receive<Setting::MidiBridgeSettings>(EndPoint::ExternalMidiOverIPBridge,
                                       [&](const Setting::MidiBridgeSettings &msg) { enableMidi = msg.enable; });

  if(outputHandle)
  {
    receive<SimpleMessage>(endPoint, [=](const auto &msg) {
      sendToExternalDevice(outputHandle, msg);
    });
  }

  pipe(cancelPipe);

  if(inputHandle)
  {
    sender = std::thread([&] { readMidi(cancelPipe[0], inputHandle); });
  }

  if(inputHandle || outputHandle)
  {
    runMainLoop();

    if(sender.joinable())
      sender.join();

    snd_rawmidi_close(outputHandle);
    snd_rawmidi_close(inputHandle);
  }

  return EXIT_SUCCESS;
}
