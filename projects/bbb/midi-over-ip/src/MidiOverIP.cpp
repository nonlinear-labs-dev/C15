#include "Options.h"
#include "AlsaDeviceMonitor.h"
#include "IO.h"

#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <glibmm.h>

Glib::RefPtr<Glib::MainLoop> theMainLoop;

void quit(int)
{
  if(theMainLoop)
    theMainLoop->quit();
}

template <typename T> T sync(const std::vector<std::string> &names, T &&io)
{
  T res;

  // save surviving ports
  for(auto &e : io)
  {
    auto it = std::find(names.begin(), names.end(), e.first);
    if(it != names.end())
      res[e.first] = std::move(e.second);
  }

  // add new ports
  for(auto &e : names)
    if(res.count(e) == 0)
      res[e] = std::make_unique<typename T::mapped_type::element_type>(e);

  return res;
}

int main(int args, char *argv[])
{
  using namespace nltools;
  using namespace nltools::msg;
  using namespace nltools::msg::Midi;
  using namespace std::chrono;

  Options options(args, argv);

  constexpr auto endPoint = EndPoint::ExternalMidiOverIPBridge;
  constexpr auto almostRealtime = threading::Priority::AlmostRealtime;
  const auto audioEngineHost = options.getAudioEngineHost();

  theMainLoop = Glib::MainLoop::create();

  nltools::msg::init({ { { EndPoint::ExternalMidiOverIPBridge, almostRealtime } },
                       { { EndPoint::ExternalMidiOverIPClient, audioEngineHost, almostRealtime } } });

  std::map<std::string, std::unique_ptr<Input> > inputs;
  std::map<std::string, std::unique_ptr<Output> > outputs;

  AlsaDeviceMonitor monitor([&](auto &ins, auto &outs) {
    inputs = sync(ins, std::move(inputs));
    outputs = sync(outs, std::move(outputs));
  });

  receive<SimpleMessage>(endPoint, [&](const SimpleMessage &msg) mutable {
    for(const auto &out : outputs)
      out.second->send(msg);
  });

  signal(SIGQUIT, quit);
  signal(SIGTERM, quit);

  theMainLoop->run();
  return EXIT_SUCCESS;
}
