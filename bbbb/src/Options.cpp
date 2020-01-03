#include "Options.h"
#include <glibmm/optiongroup.h>

namespace OptionDetail
{
  template <typename T>
  inline Glib::OptionEntry createOption(Glib::OptionGroup& ctx, const char* l, char s, const char* desc, T& dest)
  {
    Glib::OptionEntry entry;
    entry.set_long_name(l);
    entry.set_short_name(s);
    entry.set_description(desc);
    ctx.add_entry(entry, dest);
    return entry;
  }
}

Options::Options(int& argc, char**& argv)
{
  Glib::OptionGroup mainGroup("common", "common options");
  Glib::OptionContext ctx;

  auto timeStamps = OptionDetail::createOption(
      mainGroup, "timestamps", 't', "measure turn around time encoder -> playground -> oled", m_doTimeStamps);
  auto heartBeatLog
      = OptionDetail::createOption(mainGroup, "log-heart-beat", 'h', "log lpc heart beat to console", m_logHeartBeat);
  auto rawLpcLog
      = OptionDetail::createOption(mainGroup, "log-lpc-raw", 'l', "log raw lpc messages to console", m_logLpcRaw);
  auto pgHost
      = OptionDetail::createOption(mainGroup, "playground-host", 'p', "where to find the playground", m_playgroundHost);
  auto aeHost = OptionDetail::createOption(mainGroup, "audioengine-host", 'a', "where to find the audio engine",
                                           m_audioengineHost);

  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  ctx.parse(argc, argv);
}

Glib::ustring Options::getPlaygroundHost() const
{
  return m_playgroundHost;
}

Glib::ustring Options::getAudioEngineHost() const
{
  return m_audioengineHost;
}

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