#pragma once

#include "http/UpdateDocumentContributor.h"
#include <libundo/undo/Scope.h>

class PresetSettings;

class PresetSetting : public UpdateDocumentContributor
{
 private:
  typedef UpdateDocumentContributor super;

 public:
  PresetSetting(PresetSettings &parent);
  virtual ~PresetSetting();

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  bool matchesQuery(const Glib::ustring &query) const;

  virtual ustring save() const = 0;
  virtual void load(UNDO::Scope::tTransactionPtr, const Glib::ustring &v) = 0;

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;
  connection onSettingChange(slot<void, const PresetSetting *> slot);

  virtual void sendToLPC() const = 0;

 private:
  PresetSetting(const PresetSetting &other) = delete;
  PresetSetting &operator=(const PresetSetting &) = delete;

  Signal<void, const PresetSetting *> m_signal;
};
