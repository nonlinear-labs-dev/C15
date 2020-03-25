#include "Application.h"
#include "device-settings/DebugLevel.h"
#include "Options.h"
#include "playground-helpers.h"
#include <giomm.h>

int main(int numArgs, char **argv)
{
  Glib::init();
  Gio::init();

  Environment::setupLocale();

  ::signal(SIGSEGV, Environment::printStackTrace);
  ::signal(SIGILL, Environment::printStackTrace);
  ::signal(SIGBUS, Environment::printStackTrace);
  ::signal(SIGKILL, Environment::printStackTrace);

#ifdef _PROFILING
  Profiler::get().enable(true);
#endif

  {
    Application app(numArgs, argv);
    Application::get().run();
    DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  }

  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  return EXIT_SUCCESS;
}
