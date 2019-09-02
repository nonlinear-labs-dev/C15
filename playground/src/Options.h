#pragma once

#include "playground.h"
#include <giomm/file.h>

class Options
{
 public:
  Options(int &argc, char **&argv);

  const ustring & getPresetManagerPath() const;
  const ustring & getSettingsFile() const;
  const ustring & getKioskModeFile() const;
  const ustring & getHardwareTestsFolder() const;
  const ustring & getBBBB() const;
  const ustring & getLayoutFolder() const;
  const ustring & getAudioEngineHost() const;
  const ustring & getSelfPath() const;

 private:
  void setDefaults();

  bool setPMPathName(const Glib::ustring &optionName, const Glib::ustring &path, bool hasValue);
  bool setLayoutFolder(const Glib::ustring &optionName, const Glib::ustring &path, bool hasValue);
  bool makePresetManagerDirectory(Glib::RefPtr<Gio::File> file);

  Glib::ustring m_pmPath;
  Glib::ustring m_settingsFile;
  Glib::ustring m_kioskModeFile;
  Glib::ustring m_layoutFolder;
  Glib::ustring m_bbbb = "localhost";
  Glib::ustring m_audioEngineHost = "localhost";
  Glib::ustring m_selfPath;
};
