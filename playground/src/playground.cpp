#include "Application.h"
#include "testing/TestDriver.h"
#include <stdlib.h>
#include <execinfo.h>
#include <iostream>
#include <chrono>
#include <cmath>
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

std::string doubleToStringWithPrecision(double value, int precision)
{
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(precision) << value;

  int target = precision + 2;
  if(value < 0)
      target+=1;

  while(ss.str().size() != target) {
      ss << 0;
  }

  return ss.str();
}

double doubleToDoubleWithPrecision(double v, int precision)
{
    double denom = std::pow(10, precision);
    auto rounded = static_cast<long long>(v * denom);
    return rounded / denom;
}

std::string getStackTrace(const std::string& prefix)
{
  std::stringstream str;
  str << prefix << ":" << std::endl << std::endl;

  const size_t max_frames = 16;
  void* addrlist[max_frames + 1];
  guint32 addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if(addrlen != 0)
  {
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    for(guint32 i = 0; i < addrlen; i++)
      str << symbollist[i] << std::endl;

    free(symbollist);
  }
  return str.str();
}

void setupLocale()
{
  const char* desiredLocales[] = { "en_US.utf8@nonlinear", "en_US.utf8" };

  for(const auto desiredLocale : desiredLocales)
  {
    if(auto ret = setlocale(LC_ALL, desiredLocale))
    {
      if(g_strcmp0(ret, desiredLocale))
      {
        DebugLevel::warning("Desired locale was", desiredLocale, ", but current locale is:", ret);
      }
      else
      {
        DebugLevel::info("Successfully set locale to", desiredLocale);
        return;
      }
    }
  }

  DebugLevel::error("Could not set locale to any desired");
}

int main(int numArgs, char** argv)
{
  Gio::init();
  setupLocale();

  ::signal(SIGSEGV, printStackTrace);
  ::signal(SIGILL, printStackTrace);
  ::signal(SIGBUS, printStackTrace);
  ::signal(SIGKILL, printStackTrace);

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
