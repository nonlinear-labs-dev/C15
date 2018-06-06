#pragma once

#include "playground.h"
#include <giomm/file.h>

class Options
{
  public:
    Options(int &argc, char **&argv);
    virtual ~Options();

    Glib::ustring getPresetManagerPath() const;
    Glib::ustring getSettingsFile() const;
    Glib::ustring getKioskModeFile() const;
    Glib::ustring getHardwareTestsFolder() const;
    Glib::ustring getBBBB() const;
    Glib::ustring getLayoutFolder() const;

  private:
    void setDefaults();

    bool setPMPathName(const Glib::ustring &optionName, const Glib::ustring &path, bool hasValue);
    bool setLayoutFolder(const Glib::ustring &optionName, const Glib::ustring &path, bool hasValue);
    bool makePresetManagerDirectory(Glib::RefPtr<Gio::File> file);
    Glib::ustring m_pmPath;
    Glib::ustring m_settingsFile;
    Glib::ustring m_kioskModeFile;
    Glib::ustring m_bbbb = "127.0.0.1";
    Glib::ustring m_layoutFolder;
};
