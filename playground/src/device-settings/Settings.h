#pragma once

#include "playground.h"
#include "http/ContentSection.h"
#include "SettingsActions.h"
#include <tools/DelayedJob.h>
#include <tools/ScopedGuard.h>

class Application;
class Setting;

class Settings : public ContentSection
{
 private:
  typedef ContentSection super;

 public:
  typedef shared_ptr<Setting> tSettingPtr;
  typedef map<Glib::ustring, tSettingPtr> tMap;

  Settings(UpdateDocumentMaster *master);
  virtual ~Settings();

  void init();
  void reload();

  tSettingPtr getSetting(const Glib::ustring &key);
  void setSetting(const Glib::ustring &key, const Glib::ustring &value);
  void addSetting(const Glib::ustring &key, Setting *s);

  template <typename T> shared_ptr<T> getSetting()
  {
    for(auto s : m_settings)
      if(shared_ptr<T> r = dynamic_pointer_cast<T>(s.second))
        return r;

    return nullptr;
  }

  const tMap &getSettings() const;

  void handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
  Glib::ustring getPrefix() const override;

  void sendToLPC();

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

  bool isLoading() const;

 protected:
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  void save();
  void load();
  void sanitize();

  SettingsActions m_actions;
  tMap m_settings;
  DelayedJob m_saveJob;
  ScopedGuard m_isLoading;
};
