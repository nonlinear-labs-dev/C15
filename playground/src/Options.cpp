#include "Options.h"
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
  pmPath.set_description("name of the folder that stores preset-managers banks as XML files");
  mainGroup.add_entry_filename(pmPath, sigc::mem_fun(this, &Options::setPMPathName));

  OptionEntry layoutPath;
  layoutPath.set_flags(OptionEntry::FLAG_FILENAME);
  layoutPath.set_long_name("layouts");
  layoutPath.set_short_name('l');
  layoutPath.set_description("name of the folder containing the styles, controls and layouts");
  mainGroup.add_entry_filename(layoutPath, sigc::mem_fun(this, &Options::setLayoutFolder));

  OptionEntry bbbb;
  bbbb.set_long_name("bbbb");
  bbbb.set_short_name('b');
  bbbb.set_description("IP of the device running the BeagleBoneBlackBridge (bbbb)");
  mainGroup.add_entry(bbbb, m_bbbb);

  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  ctx.parse(argc, argv);
}

Options::~Options()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

void Options::setDefaults()
{
  ustring prefered = "/internalstorage/preset-manager/";

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

  Glib::ustring p = Application::get().getSelfPath();
  size_t lastSlash = p.rfind('/');
  Glib::ustring path = "/resources/Templates/";
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

bool Options::setPMPathName(const Glib::ustring &optionName, const Glib::ustring &path, bool hasValue)
{
  if(hasValue)
    m_pmPath = path;

  return true;
}

bool Options::setLayoutFolder(const Glib::ustring &optionName, const Glib::ustring &path, bool hasValue)
{
  if(hasValue)
    m_layoutFolder = path;

  return true;
}

Glib::ustring Options::getPresetManagerPath() const
{
  return m_pmPath;
}

Glib::ustring Options::getBBBB() const
{
  return m_bbbb;
}

Glib::ustring Options::getSettingsFile() const
{
  return m_settingsFile;
}

Glib::ustring Options::getKioskModeFile() const
{
  return m_kioskModeFile;
}

Glib::ustring Options::getLayoutFolder() const
{
  return m_layoutFolder;
}

Glib::ustring Options::getHardwareTestsFolder() const
{
  const char *folder = "/home/hhoegelo/hw_tests-binaries";
  //const char *folder = "/nonlinear/hw_tests-binaries";
  return folder;
}
