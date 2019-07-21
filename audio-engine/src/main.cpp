#include "Options.h"
#include "synth/SimpleSynth.h"
#include "synth/C15Synth.h"
#include "ui/CommandlinePerformanceWatch.h"

#include <nltools/logging/Log.h>
#include <nltools/StringTools.h>
#include <nltools/messaging/Messaging.h>

#include <glibmm.h>
#include <iostream>

#include <ui/C15_CLI.h>

static Glib::RefPtr<Glib::MainLoop> theMainLoop;
static std::unique_ptr<Options> theOptions;

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

const Options *getOptions()
{
  return theOptions.get();
}

void runMainLoop()
{
  theMainLoop = Glib::MainLoop::create();
  theMainLoop->run();
}

void setupMessaging(const Options *o)
{
  using namespace nltools::msg;
  Configuration conf;
  conf.offerEndpoints = { EndPoint::AudioEngine };
  conf.useEndpoints = { { EndPoint::Playground, o->getPlaygroundHost() } };
  nltools::msg::init(conf);
}

int main(int args, char *argv[])
{
  Glib::init();
  connectSignals();
  theOptions = std::make_unique<Options>(args, argv);
  setupMessaging(theOptions.get());

  if(theOptions->doMeasurePerformance())
  {
    auto synth = std::make_unique<C15Synth>();
    synth->measurePerformance(std::chrono::seconds(5));  // warm up
    auto result = synth->measurePerformance(std::chrono::seconds(5));
    nltools::Log::info("Audio engine performs at", result, "x realtime.");
    return EXIT_SUCCESS;
  }

  //auto synth = std::make_unique<SimpleSynth>();
  auto synth = std::make_unique<C15Synth>();
  C15_CLI commandLineInterface(synth.get());
  CommandlinePerformanceWatch watch(synth->getAudioOut());
  synth->start();
  runMainLoop();
  synth->stop();

  return EXIT_SUCCESS;
}
