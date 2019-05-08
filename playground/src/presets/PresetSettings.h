#pragma once

#include "http/UpdateDocumentContributor.h"
#include <libundo/undo/Scope.h>
#include <map>
#include <glib.h>

class Preset;
class PresetSetting;

class PresetSettings : public UpdateDocumentContributor
{
 private:
  typedef UpdateDocumentContributor super;

 public:
  typedef shared_ptr<PresetSetting> tSetting;
  typedef map<Glib::ustring, tSetting> tSettings;

  PresetSettings(Preset &preset);
  virtual ~PresetSettings();

  void set(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &key, const Glib::ustring &value);
  void copyFrom(UNDO::Scope::tTransactionPtr transaction, const PresetSettings &other);

  tSetting getSetting(const Glib::ustring &key);
  tSettings &getSettings();
  const tSettings &getSettings() const;

  bool matchesQuery(const Glib::ustring &query) const;

  void sendToLPC() const;

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  size_t getHash() const;

 private:
  PresetSettings(const PresetSettings &other) = delete;
  PresetSettings &operator=(const PresetSettings &) = delete;

  void add(const Glib::ustring &key, PresetSetting *s);

  tSettings m_settings;

  friend class PresetSettingsSerializer;
};
