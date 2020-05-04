#include "Options.h"
#include <glibmm/optiongroup.h>
#include <glibmm/optioncontext.h>
#include <nltools/logging/Log.h>

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

AudioEngineOptions::AudioEngineOptions(int& argc, char**& argv)
{
  Glib::OptionGroup mainGroup("common", "common options");
  Glib::OptionContext ctx;

  OptionDetail::createOption(mainGroup, "timestamps", 't', "measure turn around time encoder -> playground -> oled",
                             m_doTimeStamps);
  OptionDetail::createOption(mainGroup, "playground-host", 'p', "where to find the playground", m_playgroundHost);
  OptionDetail::createOption(mainGroup, "audioengine-host", 'a', "where to find the audio engine", m_audioengineHost);

  OptionDetail::createOption(mainGroup, "log-heart-beat", 'h', "log lpc heart beat to console", m_logHeartBeat);
  OptionDetail::createOption(mainGroup, "log-lpc-raw", 'l', "log raw lpc messages to console", m_logLpcRaw);

  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  try
  {
    ctx.parse(argc, argv);
  }
  catch(...)
  {
    std::stringstream ss;
    for(auto i = 0; i < argc; i++)
      ss << argv[i] << " ";
    nltools::Log::error(__FILE__, __FUNCTION__, __LINE__, "Could not parse args:", ss.str());
  }
}

Glib::ustring AudioEngineOptions::getPlaygroundHost() const
{
  return m_playgroundHost;
}

Glib::ustring AudioEngineOptions::getAudioEngineHost() const
{
  return m_audioengineHost;
}

bool AudioEngineOptions::doTimeStamps() const
{
  return m_doTimeStamps;
}

bool AudioEngineOptions::logLPCRaw() const
{
  return m_logLpcRaw;
}

bool AudioEngineOptions::logHeartBeat() const
{
  return m_logHeartBeat;
}
