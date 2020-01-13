#pragma once

#include "playground.h"
#include <giomm.h>

class Options
{
 public:
  Options(int &argc, char **&argv);

  const Glib::ustring &getPresetManagerPath() const;
  const Glib::ustring &getSettingsFile() const;
  const Glib::ustring &getKioskModeFile() const;
  const Glib::ustring &getHardwareTestsFolder() const;
  const Glib::ustring &getBBBB() const;
  const Glib::ustring &getLayoutFolder() const;
  const Glib::ustring &getAudioEngineHost() const;
  bool sendBBBBTurnaroundTimestamps() const;

  Glib::ustring getInstallDir() const;

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
  bool m_sendBBBTurnaroundTimes = false;
};
