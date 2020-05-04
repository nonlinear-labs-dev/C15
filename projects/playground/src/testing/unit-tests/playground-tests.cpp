#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include "Application.h"
#include "device-settings/DebugLevel.h"
#include "playground-helpers.h"
#include <giomm.h>

int main(int numArgs, char **argv)
{
  nltools::Log::setLevel(nltools::Log::Silent);

  Gio::init();

  Environment::setupLocale();

  ::signal(SIGSEGV, Environment::printStackTrace);
  ::signal(SIGILL, Environment::printStackTrace);
  ::signal(SIGBUS, Environment::printStackTrace);
  ::signal(SIGKILL, Environment::printStackTrace);

  Application app(numArgs, argv);

  nltools::Log::setLevel(nltools::Log::Error);

  app.stopWatchDog();

  std::vector<const char *> args;
  for(auto i = 0; i < numArgs; i++)
    if(argv[i] && std::string(argv[i]).find('=') == std::string::npos)
      args.emplace_back(argv[i]);

  return Catch::Session().run(args.size(), args.data());
}
