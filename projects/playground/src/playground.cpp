#include "Application.h"
#include "device-settings/DebugLevel.h"
#include "Options.h"
#include "playground-helpers.h"
#include <giomm.h>

#include <stdio.h>
#include <string.h>
#include "version.h"

#define PROGNAME "playground"
#define VERSION "--version"

static void printVersion(void)
{
  printf(PROGNAME " version %s, %s\n", GetC15Version(), GetC15Build());
}

int main(int numArgs, char **argv)
{
  if (numArgs == 2 && strncmp(argv[1], VERSION, sizeof VERSION) == 0)
  {
    printVersion();
    return 0;
  }
    
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
