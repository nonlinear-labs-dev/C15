#pragma once

#include "UndoableVector.h"
#include "AttributesOwner.h"

#include <memory>
#include <vector>
#include <map>
#include <string>

#include <serialization/PresetBankSerializer.h>
#include <tools/Uuid.h>

class Preset;
class PresetManager;
class SearchQuery;

using PresetPtr = std::unique_ptr<Preset>;

class Bank : public AttributesOwner
{
 private:
  using super = AttributesOwner;

 public:
  enum AttachmentDirection
  {
    none,
    top,
    left,
    count
  };

  explicit Bank(UpdateDocumentContributor *parent);
  Bank(UpdateDocumentContributor *parent, const Bank &other, bool ignoreUuids);
  ~Bank() override;

  void load(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> bankFolder, int numBank, int numBanks);
  SaveResult save(Glib::RefPtr<Gio::File> bankFolder);

  // supported interfaces
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  // accessors
  bool empty() const;
  const Uuid &getUuid() const;
  std::string getName(bool withFallback) const;
  std::string getX() const;
  std::string getY() const;
  const Uuid &getSelectedPresetUuid() const;
  Preset *getSelectedPreset() const;
  const Uuid &getAttachedToBankUuid() const;
  std::string getAttachDirection() const;
  size_t getNumPresets() const;
  size_t getPresetPosition(const Uuid &uuid) const;
  size_t getPresetPosition(const Preset *preset) const;
  Preset *findPreset(const Uuid &uuid) const;
  Preset *findPresetNear(const Uuid &anchorUuid, int seek) const;
  Preset *findSelectedPreset() const;
  Preset *getPresetAt(size_t idx) const;
  void forEachPreset(std::function<void(Preset *)> cb) const;
  Bank *getMasterTop() const;
  Bank *getMasterLeft() const;
  Bank *getSlaveRight() const;
  Bank *getSlaveBottom() const;
  time_t getLastChangedTimestamp() const;

  void rename(const Glib::ustring &name);
  void attachBank(UNDO::Transaction *transaction, const Uuid &otherBank, AttachmentDirection dir);
  void invalidate();

  Preset *clonePreset(const Preset *p);

  // transactions
  void copyFrom(UNDO::Transaction *transaction, const Bank *other, bool ignoreUuids);
  void setName(UNDO::Transaction *transaction, const std::string &name);
  void setUuid(UNDO::Transaction *transaction, const Uuid &uuid);
  void selectPreset(UNDO::Transaction *transaction, const Uuid &uuid);
  void selectPreset(UNDO::Transaction *transaction, size_t pos);
  void ensurePresetSelection(UNDO::Transaction *transaction);
  void setAttachedToBank(UNDO::Transaction *transaction, const Uuid &uuid);
  void setAttachedDirection(UNDO::Transaction *transaction, const std::string &direction);
  void setX(UNDO::Transaction *transaction, const std::string &x);
  void setY(UNDO::Transaction *transaction, const std::string &y);
  void updateLastModifiedTimestamp(UNDO::Transaction *transaction);

  Preset *appendPreset(UNDO::Transaction *transaction);
  Preset *appendPreset(UNDO::Transaction *transaction, std::unique_ptr<Preset> preset);
  Preset *prependPreset(UNDO::Transaction *transaction, std::unique_ptr<Preset> preset);
  Preset *insertPreset(UNDO::Transaction *transaction, size_t pos, std::unique_ptr<Preset> preset);

  Preset *appendAndLoadPreset(UNDO::Transaction *transaction, std::unique_ptr<Preset> preset);
  Preset *prependAndLoadPreset(UNDO::Transaction *transaction, std::unique_ptr<Preset> preset);
  Preset *insertAndLoadPreset(UNDO::Transaction *transaction, size_t pos, std::unique_ptr<Preset> preset);

  void movePreset(UNDO::Transaction *transaction, const Preset *toMove, const Preset *before);
  void movePresetBetweenBanks(UNDO::Transaction *transaction, Preset *presetToMove, Bank *tgtBank,
                              const Preset *presetAnchor);

  void deletePreset(UNDO::Transaction *transaction, const Uuid &uuid);
  void resolveCyclicAttachments(UNDO::Transaction *transaction);

  // algorithm
  int getHighestIncrementForBaseName(const Glib::ustring &baseName) const;
  void searchPresets(Writer &writer, const SearchQuery &query) const;
  const Glib::ustring calcStateString() const;
  Bank *getClusterMaster();

  // signals
  sigc::connection onBankChanged(sigc::slot<void> cb);

  const Preset *getFirstPreset() const;

  PresetManager *getPresetManager() const;

  bool isMidiSelectedBank() const;
  Glib::ustring getComment();

 private:
  using Attributes = std::map<std::string, std::string>;

  uint64_t loadMetadata(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> bankFolder);
  void loadPresets(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> bankFolder);
  void deleteOldPresetFiles(Glib::RefPtr<Gio::File> bankFolder);

  SaveResult saveMetadata(Glib::RefPtr<Gio::File> bankFolder);
  SaveResult savePresets(Glib::RefPtr<Gio::File> bankFolder);

  bool resolveCyclicAttachments(UNDO::Transaction *transaction, std::vector<Bank *> stackedBanks);

  UpdateDocumentContributor::tUpdateID onChange(uint64_t flags) override;
  size_t getNextPresetPosition() const;
  size_t getPreviousPresetPosition() const;

  Uuid m_uuid;
  Uuid m_attachedToBankWithUuid;
  std::string m_name;
  std::string m_serializationDate;
  std::string m_x;
  std::string m_y;
  std::string m_attachDirection;

  UndoableVector<Bank, Preset> m_presets;

  uint64_t m_lastChangedTimestamp = 0;
  tUpdateID m_metadataLastSavedForUpdateID = 0;
  tUpdateID m_presetsLastSavedForUpdateID = 0;

  Signal<void> m_sigBankChanged;

  friend class PresetBankSerializer;
  friend class PresetBankMetadataSerializer;
};

std::string to_string(Bank::AttachmentDirection dir);
Bank::AttachmentDirection toAttachDirection(const std::string &str);
