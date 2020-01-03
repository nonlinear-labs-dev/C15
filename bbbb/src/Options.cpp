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

  auto timeStamps
      = OptionDetail::createOption("timestamps", 't', "measure turn around time encoder -> playground -> oled");
  auto heartBeatLog = OptionDetail::createOption("log-heart-beat", 'h', "log lpc heart beat to console");
  auto rawLpcLog = OptionDetail::createOption("log-lpc-raw", 'l', "log raw lpc messages to console");

  auto pgHost = OptionDetail::createOption("playground-host", 'p', "where to find the playground");
  auto aeHost = OptionDetail::createOption("audioengine-host", 'a', "where to find the audio engine");

  Glib::OptionEntry pg;
  pg.set_long_name("playground-host");
  pg.set_short_name('p');
  pg.set_description("Where to find the playground");
  mainGroup.add_entry(pg, m_playgroundHost);

  Glib::OptionEntry ae;
  pg.set_long_name("audioengine-host");
  pg.set_short_name('p');
  pg.set_description("Where to find the audioengine");
  mainGroup.add_entry(ae, m_audioengineHost);

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