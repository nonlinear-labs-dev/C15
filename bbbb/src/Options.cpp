#include "Options.h"
#include <glibmm/optiongroup.h>

namespace OptionDetail
{
  inline Glib::OptionEntry createOption(const char* l, char s, const char* desc)
  {
    Glib::OptionEntry entry;
    entry.set_long_name(l);
    entry.set_short_name(s);
    entry.set_description(desc);
    return entry;
  }
}

Options::Options(int& argc, char**& argv)
{
  Glib::OptionGroup mainGroup("common", "common options");
  Glib::OptionContext ctx;
  auto timeStamps = OptionDetail::createOption("timestamps", 't',
                                               "measure turn around time encoder -> playground -> oled");
  auto heartBeatLog = OptionDetail::createOption( "log-heart-beat", 'h', "log lpc heart beat to console");
  auto rawLpcLog = OptionDetail::createOption( "log-lpc-raw", 'l', "log raw lpc messages to console");

  mainGroup.add_entry(timeStamps, m_doTimeStamps);
  mainGroup.add_entry(heartBeatLog, m_logHeartBeat);
  mainGroup.add_entry(rawLpcLog, m_logLpcRaw);

  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  ctx.parse(argc, argv);
}

Options::~Options() = default;

bool Options::doTimeStamps() const
{
  return m_doTimeStamps;
}

bool Options::logLPCRaw() const
{
  return m_logLpcRaw;
}

bool Options::logHeartBeat() const
{
  return m_logHeartBeat;
}