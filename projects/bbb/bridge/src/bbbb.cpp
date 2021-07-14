#include "bbbb.h"
#include <stdlib.h>
#include <execinfo.h>
#include <iostream>
#include <chrono>
#include <nltools/logging/Log.h>
#include <giomm.h>
#include "Application.h"
#ifdef _DEVELOPMENT_PC
#include <ui/Window.h>
#endif

void printStackTrace(int i)
{
  nltools::Log::error("Crash signal caught!");

  const size_t max_frames = 64;
  void* addrlist[max_frames + 1];

  // retrieve current stack addresses
  guint32 addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if(addrlen == 0)
  {
    nltools::Log::error("");
    return;
  }

  nltools::Log::error("\n\nThe stack trace:");

  // create readable strings to each frame. __attribute__((no_instrument_function))
  char** symbollist = backtrace_symbols(addrlist, addrlen);

  // print the stack trace.
  for(guint32 i = 0; i < addrlen; i++)
    nltools::Log::error(symbollist[i]);

  free(symbollist);
  exit(EXIT_FAILURE);
}

int main(int numArgs, char** argv)
{
  Gio::init();

  ::signal(SIGSEGV, printStackTrace);
  ::signal(SIGILL, printStackTrace);
  ::signal(SIGBUS, printStackTrace);

  {
    nltools::Log::setLevel(nltools::Log::Warning);
    Application app(numArgs, argv);
    Application::get().run();
  }

  return EXIT_SUCCESS;
}

void printCurrentTime(const char* name)
{
  auto now = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
  printCurrentTime(name, ms);
}

void printCurrentTime(const char* name, int64_t ms)
{
  std::cout << "Performance time stamp at " << name << ": " << ms << std::endl;
}
