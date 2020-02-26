#define CATCH_CONFIG_RUNNER

#include <third-party/include/catch.hpp>
#include "Application.h"
#include "testing/TestDriver.h"
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

  Application app(0, nullptr);
  nltools::Log::setLevel(nltools::Log::Error);

  app.stopWatchDog();
  return Catch::Session().run(numArgs, argv);
}
