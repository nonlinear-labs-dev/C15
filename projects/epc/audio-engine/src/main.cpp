#include "AudioEngineOptions.h"
#include "synth/C15Synth.h"
#include "synth/CPUBurningSynth.h"
#include "ui/CommandlinePerformanceWatch.h"
#include "io/MidiHeartBeat.h"
#include "io/audio/AudioOutputMock.h"
#include "io/audio/AlsaAudioOutput.h"
#include "io/midi/MidiInputMock.h"
#include "io/midi/AlsaMidiInput.h"

#include "recorder/Recorder.h"

#include <nltools/logging/Log.h>
#include <nltools/StringTools.h>
#include <nltools/messaging/Messaging.h>

#include <glibmm.h>
#include <iostream>

#include "ui/C15_CLI.h"

static Glib::RefPtr<Glib::MainLoop> theMainLoop;
static std::unique_ptr<AudioEngineOptions> theOptions;

static void quit(int)
{
  if(theMainLoop)
    theMainLoop->quit();
}

static void connectSignals()
{
  signal(SIGINT, quit);
  signal(SIGQUIT, quit);
  signal(SIGTERM, quit);
}

static const AudioEngineOptions *getOptions()
{
  return theOptions.get();
}

static void runMainLoop()
{
  theMainLoop = Glib::MainLoop::create();
  theMainLoop->run();
}

static void setupMessaging(const AudioEngineOptions *o)
{
  using namespace nltools::msg;

  auto highPrio = nltools::threading::Priority::AlmostRealtime;

  Configuration conf;
  conf.offerEndpoints = { { EndPoint::AudioEngine }, { EndPoint::ExternalMidiOverIPClient, highPrio } };
  conf.useEndpoints = { { EndPoint::Playground, o->getPlaygroundHost() },
                        { EndPoint::ExternalMidiOverIPBridge, o->getMidiBridgeHost(), highPrio } };

  nltools::msg::init(conf);
}

static std::unique_ptr<Synth> createSynth(AudioEngineOptions *options)
{
  if(options->getNumCpuBurningSines())
    return std::make_unique<CPUBurningSynth>(options);

  return std::make_unique<C15Synth>(options);
}

static std::unique_ptr<C15_CLI> createCLI(Synth *synth, AudioOutput *audioOut)
{
  if(auto c15 = dynamic_cast<C15Synth *>(synth))
    return std::make_unique<C15_CLI>(c15, audioOut);

  return nullptr;
}

static int measurePerformance()
{
  auto synth = std::make_unique<C15Synth>(theOptions.get());
  synth->measurePerformance(std::chrono::seconds(5));  // warm up
  auto result = std::get<1>(synth->measurePerformance(std::chrono::seconds(5)));
  nltools::Log::info("Audio engine performs at", result, "x realtime.");
  return EXIT_SUCCESS;
}

static std::unique_ptr<Recorder> createRecorder(int sampleRate)
{
  return std::make_unique<Recorder>(sampleRate);
}

static std::unique_ptr<AudioOutput> createAudioOut(const std::string &name, Synth *synth, Recorder *recorder)
{
  return name.empty()
      ? nullptr
      : std::make_unique<AlsaAudioOutput>(theOptions.get(), name, [synth, recorder](auto buf, auto length) {
          synth->process(buf, length);
          recorder->process(buf, length);
        });
}

static std::unique_ptr<MidiInput> createMidiIn(const std::string &name, Synth *synth)
{
  return name.empty() ? nullptr
                      : std::make_unique<AlsaMidiInput>(name, [synth](auto event) { synth->pushMidiEvent(event); });
}

static std::unique_ptr<MidiInput> createTCDIn(const std::string &name, Synth *synth)
{
  return name.empty() ? nullptr
                      : std::make_unique<AlsaMidiInput>(name, [synth](auto event) { synth->pushTcdEvent(event); });
}

template <typename... A> static void start(A &... a)
{
  auto start = [](auto &p) {
    if(p)
      p->start();
  };
  (start(a), ...);
}

template <typename... A> static void stop(A &... a)
{
  auto stop = [](auto &p) {
    if(p)
      p->stop();
  };
  (..., stop(a));
}

int main(int args, char *argv[])
{
  Glib::init();
  connectSignals();

  testParallelData();

  theOptions = std::make_unique<AudioEngineOptions>(args, argv);
  setupMessaging(theOptions.get());

  if(theOptions->doMeasurePerformance())
    return measurePerformance();

  auto synth = createSynth(theOptions.get());
  auto recorder = createRecorder(theOptions->getSampleRate());
  auto audioOut = createAudioOut(theOptions->getAudioOutputDeviceName(), synth.get(), recorder.get());
  auto midiIn = createMidiIn(theOptions->getMidiInputDeviceName(), synth.get());
  auto tcdIn = createTCDIn(theOptions->getTcdInputDeviceName(), synth.get());
  auto cli = createCLI(synth.get(), audioOut.get());

  MidiHeartBeat heartbeat(theOptions->getHeartBeatDeviceName());
  CommandlinePerformanceWatch watch(audioOut.get());

  start(audioOut, midiIn, tcdIn);
  runMainLoop();
  stop(audioOut, midiIn, tcdIn);

  return EXIT_SUCCESS;
}
