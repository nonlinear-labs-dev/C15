#include "Application.h"
#include "testing/TestDriver.h"
#include <stdlib.h>
#include <execinfo.h>
#include <iostream>
#include <chrono>
#include "device-settings/DebugLevel.h"
#include "profiling/Profiler.h"

void printStackTrace(int i)
{
  DebugLevel::warning("Crash signal caught!");

  const size_t max_frames = 64;
  void* addrlist[max_frames + 1];

  // retrieve current stack addresses
  guint32 addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if(addrlen == 0)
  {
    DebugLevel::warning("");
    return;
  }

  DebugLevel::warning("\n\nThe stack trace:");

  // create readable strings to each frame. __attribute__((no_instrument_function))
  char** symbollist = backtrace_symbols(addrlist, addrlen);

  // print the stack trace.
  for(guint32 i = 0; i < addrlen; i++)
    DebugLevel::warning(symbollist[i]);

  free(symbollist);
  exit(EXIT_FAILURE);
}

void setupLocale()
{
  const auto desiredLocale = "en_US.utf8";

  if(auto ret = setlocale(LC_ALL, desiredLocale))
  {
    if(g_strcmp0(ret, desiredLocale))
    {
      DebugLevel::warning("Desired locale was", desiredLocale, ", but current locale is:", ret);
    }
    else
    {
      DebugLevel::info("Successfully set locale to", desiredLocale);
    }
  }
  else
  {
    DebugLevel::error("Could not set locale to", desiredLocale);
  }
}

int main(int numArgs, char** argv)
{
  Gio::init();
  setupLocale();

  ::signal(SIGSEGV, printStackTrace);
  ::signal(SIGILL, printStackTrace);
  ::signal(SIGBUS, printStackTrace);

  //Profiler::get ().enable (true);

#ifdef _TESTS
  TestDriverBase::doTests(numArgs, argv);
#endif

  {
    Application app(numArgs, argv);
    Application::get().run();
    DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  }

  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  return EXIT_SUCCESS;
}
