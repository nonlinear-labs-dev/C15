#include "AudioEngineOptions.h"
#include "synth/C15Synth.h"
#include "synth/CPUBurningSynth.h"
#include "ui/CommandlinePerformanceWatch.h"
#include "io/MidiHeartBeat.h"
#include "io/flac/StreamServer.h"

#include <nltools/logging/Log.h>
#include <nltools/StringTools.h>
#include <nltools/messaging/Messaging.h>

#include <glibmm.h>
#include <iostream>

#include "ui/C15_CLI.h"

static Glib::RefPtr<Glib::MainLoop> theMainLoop;
static std::unique_ptr<AudioEngineOptions> theOptions;

void quit(int)
{
  if(theMainLoop)
    theMainLoop->quit();
}

void connectSignals()
{
  signal(SIGINT, quit);
  signal(SIGQUIT, quit);
  signal(SIGTERM, quit);
}

const AudioEngineOptions *getOptions()
{
  return theOptions.get();
}

void runMainLoop()
{
  theMainLoop = Glib::MainLoop::create();
  theMainLoop->run();
}

void setupMessaging(const AudioEngineOptions *o)
{
  using namespace nltools::msg;

  auto highPrio = nltools::threading::Priority::AlmostRealtime;

  Configuration conf;
  conf.offerEndpoints = { { EndPoint::AudioEngine }, { EndPoint::ExternalMidiOverIPClient, highPrio } };
  conf.useEndpoints = { { EndPoint::Playground, o->getPlaygroundHost() },
                        { EndPoint::ExternalMidiOverIPBridge, o->getMidiBridgeHost(), highPrio } };

  nltools::msg::init(conf);
}

std::unique_ptr<Synth> createSynth(AudioEngineOptions *options)
{
  if(options->getNumCpuBurningSines())
    return std::make_unique<CPUBurningSynth>(options);

  return std::make_unique<C15Synth>(options);
}

std::unique_ptr<C15_CLI> createCLI(Synth *synth)
{
  if(auto c15 = dynamic_cast<C15Synth *>(synth))
    return std::make_unique<C15_CLI>(c15);

  return nullptr;
}

int main(int args, char *argv[])
{
  Glib::init();
  connectSignals();

  testParallelData();

  theOptions = std::make_unique<AudioEngineOptions>(args, argv);
  setupMessaging(theOptions.get());

  if(theOptions->doMeasurePerformance())
  {
    auto synth = std::make_unique<C15Synth>(theOptions.get());
    synth->measurePerformance(std::chrono::seconds(5));  // warm up
    auto result = std::get<1>(synth->measurePerformance(std::chrono::seconds(5)));
    nltools::Log::info("Audio engine performs at", result, "x realtime.");
    return EXIT_SUCCESS;
  }

  auto synth = createSynth(theOptions.get());
  auto cli = createCLI(synth.get());

  CommandlinePerformanceWatch watch(synth->getAudioOut());
  StreamServer flacStreamServer(synth.get());

  synth->start();
  MidiHeartBeat heartbeat(theOptions->getHeartBeatDeviceName());
  runMainLoop();
  synth->stop();

  return EXIT_SUCCESS;
}
