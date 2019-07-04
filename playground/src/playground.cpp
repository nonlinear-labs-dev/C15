#include "Application.h"
#include "testing/TestDriver.h"
#include <stdlib.h>
#include <execinfo.h>
#include <iostream>
#include <chrono>
#include "device-settings/DebugLevel.h"
#include "profiling/Profiler.h"
#include "Options.h"
#include <nltools/messaging/Messaging.h>
#include <nltools/StringTools.h>

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

  DebugLevel::error("Collected StackTrace: ", str.str());
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

void setupMessaging(const Options* options)
{
  using namespace nltools::msg;

  auto bbbb = options->getBBBB();

  Configuration conf;
  conf.offerEndpoints = { EndPoint::Playground };
  conf.useEndpoints = { { EndPoint::Playground }, { EndPoint::AudioEngine },    { EndPoint::Lpc, bbbb },
                        { EndPoint::Oled, bbbb }, { EndPoint::PanelLed, bbbb }, { EndPoint::RibbonLed, bbbb } };
  nltools::msg::init(conf);
}

int main(int numArgs, char** argv)
{
  Gio::init();
  auto options = std::make_unique<Options>(numArgs, argv);

  setupMessaging(options.get());

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
    Application app(std::move(options));
    Application::get().run();
    DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  }

  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  return EXIT_SUCCESS;
}
