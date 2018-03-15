#pragma once

#include "playground.h"
#include "libundo/undo/Scope.h"
#include "tools/Uuid.h"
#include "http/UpdateDocumentContributor.h"
#include "parameters/Parameter.h"
#include "tools/Signal.h"
#include <atomic>
#include <map>
#include <tools/BackgroundJobs.h>
#include <presets/Preset.h>
#include <tuple>

class Preset;
class Writer;
class EditBuffer;
class PresetManager;

class PresetBank : public UpdateDocumentContributor,
    public AttributesOwner,
    public enable_shared_from_this<PresetBank>,
    public sigc::trackable
{
  public:
    PresetBank(PresetManager *parent);
    virtual ~PresetBank();

    PresetManager *getParent();
    const PresetManager *getParent() const;

    typedef shared_ptr<Preset> tPresetPtr;

    enum AttachmentDirection
    {
      none = 0, top = 1, left = 2
    };
    const Glib::ustring directionEnumToString(AttachmentDirection direction) const;

    struct Attachment
    {
        Attachment(Glib::ustring uuid, AttachmentDirection direction) :
            uuid(uuid),
            direction(direction)
        {
        }
        Glib::ustring uuid;
        AttachmentDirection direction;
    };

    Glib::ustring getName(bool withFallback) const;
    size_t getNumPresets() const;
    tPresetPtr getPreset(size_t pos);
    const tPresetPtr getPreset(size_t pos) const;
    const vector<tPresetPtr> &getPresets() const;
    const Uuid &getUuid() const;
    bool containsPreset(const Uuid &uuid) const;
    size_t getPresetPosition(const Uuid &uuid) const;
    const tPresetPtr getPreset(const Uuid &uuid) const;
    tPresetPtr getPreset(const Uuid &uuid);
    Glib::ustring getSelectedPreset() const;
    const tPresetPtr findPresetByName(const Glib::ustring &name) const;
    SaveResult save(RefPtr<Gio::File> bankFolder);
    int getHighestIncrementForBaseName(const Glib::ustring &baseName) const;
    const Glib::ustring calcStateString() const;

    bool setSelectedPreset(Glib::ustring uuid);
    void assignDefaultPosition();
    void undoableAssignDefaultPosition(shared_ptr<UNDO::Transaction> transaction);

    void loadSync(shared_ptr<UNDO::Transaction> transaction, RefPtr<Gio::File> bankFolder, int numBank, int numBanks);
    void undoableSetName(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &name);
    void undoableSetName(const Glib::ustring &name);
    void undoableSetPosition(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &x, const Glib::ustring &y);
    void undoableMovePreset(size_t from, size_t to);
    void undoableStorePreset(size_t pos, shared_ptr<EditBuffer> editBuffer);
    void undoableInsertPreset(UNDO::Scope::tTransactionPtr transaction, int pos);
    void undoableSelect(UNDO::Scope::tTransactionPtr transaction);

    void undoableAppendPreset(UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid);
    void undoableDeletePreset(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &uuid);
    void undoableOverwritePreset(UNDO::Scope::tTransactionPtr transaction, size_t pos, tPresetPtr preset);
    void undoableOverwritePreset(UNDO::Scope::tTransactionPtr transaction, Preset *presetToOverwrite, Preset *src);
    void undoableSelectPreset(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &uuid);
    void undoableMovePosition(UNDO::Scope::tTransactionPtr transaction, double x, double y);
    void undoableEnsurePresetSelection(UNDO::Scope::tTransactionPtr transaction);
    shared_ptr<Preset> undoableExpropriatePreset(UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid);

    void undoableAdoptPreset(UNDO::Scope::tTransactionPtr transaction, size_t pos, shared_ptr<Preset> preset);
    void undoableAttachBank(UNDO::Scope::tTransactionPtr transaction, Glib::ustring daddyUuid, AttachmentDirection dir);
    void undoableDetachBank(UNDO::Scope::tTransactionPtr transaction);

    void setUuid(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &uuid);
    void copyFrom(UNDO::Scope::tTransactionPtr transaction, shared_ptr<PresetBank> other, bool ignoreUUIDs);

    void undoableIncPresetSelection(int inc, ButtonModifiers modifiers);

    void undoableSort(UNDO::Scope::tTransactionPtr transaction, bool asc);

    void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
    void searchPresets(Writer &writer, const SearchQuery &query) const;
    uint64_t getLastChangedTimestamp() const;

    const Glib::ustring &getX() const;
    const Glib::ustring &getY() const;
    const Attachment &getAttached() const;
    void setAttachedTo(UNDO::Scope::tTransactionPtr transaction, Glib::ustring attachedTo);
    void setAttachedDirection(UNDO::Scope::tTransactionPtr transaction, AttachmentDirection direction);

    void resolveCyclicAttachments(UNDO::Scope::tTransactionPtr transaction);
    bool resolveCyclicAttachments(std::vector<PresetBank*> stackedBanks, UNDO::Scope::tTransactionPtr transaction);

    PresetBank *getClusterMaster();

    virtual tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

    // CALLBACKS
    sigc::connection onBankChanged(sigc::slot<void> slot);

  private:
    void resetAttached(UNDO::Scope::tTransactionPtr transaction);
    Glib::ustring generateHumanReadablePresetName(size_t pos) const;
    void setName(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &name);
    void setX(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &x);
    void setY(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &y);

    void signalBankChanged();

    tPresetPtr createPreset();
    void insertPreset(vector<tPresetPtr>::iterator it);
    void insertPreset(vector<tPresetPtr>::iterator it, tPresetPtr ptr);

    bool sendBankChangedOnIdle();

    uint64_t loadMetadata(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> bankFolder);
    void loadPresets(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> bankFolder);
    void deleteOldPresetFiles(RefPtr<Gio::File> bankFolder);
    SaveResult savePresets(RefPtr<Gio::File> bankFolder);
    SaveResult saveMetadata(RefPtr<Gio::File> bankFolder);

    std::pair<double, double> calcDefaultPosition() const;

    Glib::ustring m_name;
    Glib::ustring m_X = "0";
    Glib::ustring m_Y = "0";
    Attachment m_attachment;

    Signal<void> m_signalBankChanged;
    vector<tPresetPtr> m_presets;

    Uuid m_uuid;
    Glib::ustring m_selectedPresetUUID;

    atomic<bool> m_signalBankChangedPending;
    tUpdateID m_metadataLastSavedForUpdateID = 0;
    tUpdateID m_presetsLastSavedForUpdateID = 0;

    uint64_t m_lastChangedTimestamp = 0;

    friend class PresetBankSerializer;
    friend class PresetOrderSerializer;
    friend class PresetBankMetadataSerializer;
};
