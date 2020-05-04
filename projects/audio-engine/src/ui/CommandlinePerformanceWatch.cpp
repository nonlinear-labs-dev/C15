#include "CommandlinePerformanceWatch.h"
#include "io/audio/AudioOutput.h"
#include <nltools/logging/Log.h>
#include <glibmm.h>
#include <unistd.h>

CommandlinePerformanceWatch::CommandlinePerformanceWatch(AudioOutput *device)
    : m_device(device)
{
  if(isatty(fileno(stdout)))
  {
    Glib::MainContext::get_default()->signal_timeout().connect_seconds(
        sigc::mem_fun(this, &CommandlinePerformanceWatch::printPerformance), 1);
  }
}

bool CommandlinePerformanceWatch::printPerformance()
{
  const auto &p = m_device->exhaustPerformance();

  char txt[256];
  auto avg = (p.num) ? p.sum / p.num : 0;

  sprintf(txt, "[%3.1f%% ... %3.1f%% ... %3.1f%% ... %3.1f%% ... %3.1f%%]", 100 * p.overallMin, 100 * p.min, 100 * avg,
          100 * p.max, 100 * p.overallMax);
  nltools::Log::output<nltools::Log::LogMode::InsertSpaces>("\rPerformance:", txt, "#>");
  nltools::Log::flush();
  return true;
}
