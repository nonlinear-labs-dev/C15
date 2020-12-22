#pragma once

#include "AttributesOwner.h"
#include "PresetDualParameterGroups.h"
#include "PresetParameterGroup.h"
#include "ParameterId.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <tools/Uuid.h>
#include <tools/Signal.h>
#include <set>

class EditBuffer;
class Bank;
class PresetSerializer;
class ParameterGroupsSerializer;
class SearchQuery;

class PresetParameterGroup;
class PresetParameter;
class PresetParameterGroupsSerializer;

class GroupId;

class Preset : public PresetDualParameterGroups
{
 private:
  using super = PresetDualParameterGroups;

 public:
  Preset(UpdateDocumentContributor *parent);
  Preset(UpdateDocumentContributor *parent, const Preset &other, bool ignoreUuids);
  Preset(UpdateDocumentContributor *parent, const EditBuffer &editBuffer, bool copyUUID = false);
  Preset(UpdateDocumentContributor *parent, const EditBuffer &editBuffer, Uuid uuid);
  ~Preset() override;

  // supported interfaces
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  void load(UNDO::Transaction *transaction, const Glib::RefPtr<Gio::File> &presetPath);
  bool save(const Glib::RefPtr<Gio::File> &bankPath);
  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;
  void setAttribute(UNDO::Transaction *transaction, const std::string &key, const Glib::ustring &value) override;
  void copyFrom(UNDO::Transaction *transaction, const AttributesOwner *other) override;
  void clear(UNDO::Transaction *transaction) override;
  void invalidate();
  size_t getNumGroups(const VoiceGroup &vg) const;

  SoundType getType() const;
  Glib::ustring getVoiceGroupName(VoiceGroup vg) const;
  void undoableSetVoiceGroupName(UNDO::Transaction *transaction, VoiceGroup vg, const Glib::ustring &name);

  // accessors
  const Uuid &getUuid() const;
  Glib::ustring getName() const;
  bool isDual() const;
  Glib::ustring getDisplayNameWithSuffixes(bool addSpace) const;

  PresetParameter *findParameterByID(ParameterId id, bool throwIfMissing) const;
  PresetParameterGroup *findParameterGroup(const GroupId &id) const;

  template <VoiceGroup VG> void forEachParameter(const std::function<void(PresetParameter *)> &cb)
  {
    for(auto &g : m_parameterGroups[static_cast<int>(VG)])
      for(auto &p : g.second->getParameters())
        cb(p.second.get());
  }

  void forEachParameter(const std::function<void(PresetParameter *)> &cb);
  void forEachParameter(const std::function<void(const PresetParameter *)> &cb) const;

  std::vector<std::pair<GroupId, const PresetParameterGroup *>> getGroups(const VoiceGroup &vg) const;

  // transactions
  void copyFrom(UNDO::Transaction *transaction, const Preset *other, bool ignoreUuid);
  void copyFrom(UNDO::Transaction *transaction, EditBuffer *edit);
  void copyVoiceGroup1IntoVoiceGroup2(UNDO::Transaction *transaction, std::optional<std::set<GroupId>> whiteList);
  void setUuid(UNDO::Transaction *transaction, const Uuid &uuid);
  void setName(UNDO::Transaction *transaction, const Glib::ustring &name);
  void setType(UNDO::Transaction *transaction, SoundType type);
  void guessName(UNDO::Transaction *transaction);
  void setAutoGeneratedAttributes(UNDO::Transaction *transaction);

  // algorithms
  Glib::ustring buildUndoTransactionTitle(const Glib::ustring &prefix) const;
  bool matchesQuery(const SearchQuery &query) const;

  void writeDiff(Writer &writer, const Preset *other, VoiceGroup vgOfThis, VoiceGroup vgOfOther) const;

  // signals
  sigc::connection onChanged(sigc::slot<void> cb);

 Glib::ustring getComment();private:
  void lockAutoGeneratedAttributes();
  void unlockAutoGeneratedAttributes();

  EditBuffer *getEditBuffer() const;
  size_t getHash() const = delete;
  void updateBanksLastModifiedTimestamp(UNDO::Transaction *transaction);

  void writeGroups(Writer &writer, const Preset *other, VoiceGroup vgOfThis, VoiceGroup vgOfOther) const;
  Uuid m_uuid;
  Glib::ustring m_name;
  std::array<Glib::ustring, 2> m_voiceGroupLabels;

  tUpdateID m_lastSavedUpdateID = 0;
  int m_autoGeneratedAttributesLockCount = 0;

  Signal<void> m_onChanged;
  friend class PresetSerializer;
  friend class PresetSettingsSerializer;
  friend class RecallEditBufferSerializer;
  friend class PresetParameterGroupsSerializer;
  friend class PresetAutoGeneratedAttributesLock;

  friend class RecallParameterGroups;

  PresetParameterGroup *findOrCreateParameterGroup(const GroupId &id);
  bool isMonoActive() const;
  bool isUnisonActive() const;
};
