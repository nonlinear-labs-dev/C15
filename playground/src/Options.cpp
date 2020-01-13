#include "Options.h"
#include "CompileTimeOptions.h"
#include <glibmm/optiongroup.h>
#include "device-settings/DebugLevel.h"
#include "Application.h"

Options::Options(int &argc, char **&argv)
{
  setDefaults();

  OptionGroup mainGroup("common", "common options");
  OptionContext ctx;

  OptionEntry pmPath;
  pmPath.set_flags(OptionEntry::FLAG_FILENAME);
  pmPath.set_long_name("pm-path");
  pmPath.set_short_name('p');
  pmPath.set_description("Name of the folder that stores preset-managers banks as XML files");
  mainGroup.add_entry_filename(pmPath, sigc::mem_fun(this, &Options::setPMPathName));

  OptionEntry layoutPath;
  layoutPath.set_flags(OptionEntry::FLAG_FILENAME);
  layoutPath.set_long_name("layouts");
  layoutPath.set_short_name('l');
  layoutPath.set_description("name of the folder containing the styles, controls and layouts");
  mainGroup.add_entry_filename(layoutPath, sigc::mem_fun(this, &Options::setLayoutFolder));

  OptionEntry bbbb;
  bbbb.set_long_name("bbbb-host");
  bbbb.set_short_name('b');
  bbbb.set_description("Where to find the bbbb");
  mainGroup.add_entry(bbbb, m_bbbb);

  OptionEntry ae;
  ae.set_long_name("audio-engine-host");
  ae.set_short_name('a');
  ae.set_description("Where to find the audio-engine");
  mainGroup.add_entry(ae, m_audioEngineHost);

  OptionEntry turn;
  turn.set_long_name("bbbb-turnaround-times");
  turn.set_short_name('t');
  turn.set_arg_description("measure turn around times for knob to BOLED messages");
  mainGroup.add_entry(turn, m_sendBBBTurnaroundTimes);

  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  ctx.parse(argc, argv);
}

void Options::setDefaults()
{
  Glib::ustring prefered = "/internalstorage/preset-manager/";

  auto file = Gio::File::create_for_path(prefered);

  if(file->query_exists() || makePresetManagerDirectory(file))
  {
    m_pmPath = prefered;
  }
  else
  {
    m_pmPath = "./preset-manager/";
  }

  m_settingsFile = "./settings.xml";
  m_kioskModeFile = "./kiosk-mode.stamp";

  Glib::ustring p = getInstallDir();
  size_t lastSlash = p.rfind('/');
  Glib::ustring path = "/resources/templates/";
  p = p.substr(0, lastSlash) + path;
  m_layoutFolder = p;
}

bool Options::makePresetManagerDirectory(Glib::RefPtr<Gio::File> file)
{
  try
  {
    return file->make_directory();
  }
  catch(...)
  {
  }
  return false;
}

bool Options::setPMPathName(const Glib::ustring &, const Glib::ustring &path, bool hasValue)
{
  if(hasValue)
    m_pmPath = path;

  return true;
}

bool Options::setLayoutFolder(const Glib::ustring &, const Glib::ustring &path, bool hasValue)
{
  if(hasValue)
    m_layoutFolder = path;

  return true;
}

const Glib::ustring &Options::getPresetManagerPath() const
{
  return m_pmPath;
}

const Glib::ustring &Options::getBBBB() const
{
  return m_bbbb;
}

const Glib::ustring &Options::getAudioEngineHost() const
{
  return m_audioEngineHost;
}

Glib::ustring Options::getInstallDir() const
{
  return getInstallPrefix();
}

const Glib::ustring &Options::getSettingsFile() const
{
  return m_settingsFile;
}

const Glib::ustring &Options::getKioskModeFile() const
{
  return m_kioskModeFile;
}

const Glib::ustring &Options::getLayoutFolder() const
{
  return m_layoutFolder;
}

bool Options::sendBBBBTurnaroundTimestamps() const
{
  return m_sendBBBTurnaroundTimes;
}
