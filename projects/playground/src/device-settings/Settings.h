#pragma once

#include "playground.h"
#include "http/ContentSection.h"
#include <tools/DelayedJob.h>
#include <tools/ScopedGuard.h>
#include <utility>
#include <map>

class Application;
class Setting;
class SettingsActions;

class Settings : public ContentSection
{
 private:
  typedef ContentSection super;

 public:
  typedef std::shared_ptr<Setting> tSettingPtr;
  typedef std::map<Glib::ustring, tSettingPtr> tMap;

  Settings(UpdateDocumentMaster *master);
  virtual ~Settings();

  void init();
  void reload();

  tSettingPtr getSetting(const Glib::ustring &key);
  void setSetting(const Glib::ustring &key, const Glib::ustring &value);
  void addSetting(const Glib::ustring &key, Setting *s);

  template <typename T> std::shared_ptr<T> getSetting()
  {
    for(auto &s : m_settings)
      if(std::shared_ptr<T> r = std::dynamic_pointer_cast<T>(s.second))
        return r;

    return nullptr;
  }

  const tMap &getSettings() const;

  void handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
  Glib::ustring getPrefix() const override;

  void onEditBufferSent();

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

  bool isLoading() const;

 protected:
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  void save();
  void load();
  void sanitize();

  std::unique_ptr<SettingsActions> m_actions;
  tMap m_settings;
  DelayedJob m_saveJob;
  ScopedGuard m_isLoading;
};
