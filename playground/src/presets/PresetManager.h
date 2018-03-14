#pragma once

#include <giomm/file.h>
#include <http/NetworkRequest.h>
#include <http/UpdateDocumentContributor.h>
#include <libundo/undo/Scope.h>
#include "tools/RecursionGuard.h"
#include <tools/Expiration.h>
#include <tools/Signal.h>
#include <tools/Throttler.h>
#include <tools/Uuid.h>
#include <xml/Writer.h>
#include <xml/InStream.h>
#include <list>
#include <vector>

class Preset;

class Application;
class Writer;
class XmlReader;
class PresetBank;
class EditBuffer;
class RPCActionManager;

class PresetManager : public ContentSection
{
  public:
    enum moveDirection { LeftByOne, RightByOne };
    enum class presetInfoSearchFields : short {name, comment, devicename};


    typedef function<SaveResult ()> tSaveSubTask;

    PresetManager (UpdateDocumentContributor *parent);
    virtual ~PresetManager ();

    void init ();
    void stress(int numTransactions);
    void reload ();

    virtual Glib::ustring getPrefix () const override;
    virtual void handleHTTPRequest (shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;

    typedef shared_ptr<PresetBank> tBankPtr;
    typedef shared_ptr<Preset> tPresetPtr;

    // getters
    shared_ptr<EditBuffer> getEditBuffer ();
    size_t getNumBanks () const;
    tBankPtr getBank (int pos);
    const tBankPtr getBank (int pos) const;
    const vector<tBankPtr> &getBanks () const;

    int calcOrderNumber (const PresetBank *bank) const;
    int calcBankIndex (const PresetBank *bank) const;

    tPresetPtr getSelectedPreset() const;

    // undo transactions
    tBankPtr addBank (UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &x, const Glib::ustring &y);
    tBankPtr addBank (UNDO::Scope::tTransactionPtr transaction, bool autoSelect);

    void undoableClear ();
    void undoableClear (UNDO::Scope::tTransactionPtr transaction);
    void undoableDeleteBank (UNDO::Scope::tTransactionPtr transaction, const tBankPtr bank);

    void undoableStoreInitSound (UNDO::Scope::tTransactionPtr transaction);
    void undoableLoadInitSound (UNDO::Scope::tTransactionPtr transaction);
    void undoableResetInitSound (UNDO::Scope::tTransactionPtr transaction);
    void undoableChangeBankOrder(UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid, moveDirection direction);
    void undoableSetOrderNumber(UNDO::Scope::tTransactionPtr transaction, tBankPtr bank, int newOrderNumber);

    void writeDocument (Writer &writer, tUpdateID knownRevision) const override;
    void searchPresets (Writer &writer, const Glib::ustring &query, const Glib::ustring &mode, std::vector<presetInfoSearchFields> &&fieldsToSearch) const;

    void load ();
    bool isLoading () const;

    tBankPtr findBankWithPreset (const Glib::ustring &presetUUID);
    tPresetPtr findPreset (const Glib::ustring &presetUUID);

    tBankPtr findBank (const Glib::ustring &uuid) const;
    void undoableDeleteSelectedBank (UNDO::Scope::tTransactionPtr transaction);
    tBankPtr getSelectedBank () const;

    void undoableSelectBank (const Glib::ustring &uuid);
    bool undoableSelectBank (UNDO::Scope::tTransactionPtr transaction, int idx);
    void undoableSelectBank (UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &uuid);
    void undoableSelectNext ();
    void undoableSelectPrevious ();

    sigc::connection onBankSelection (sigc::slot<void, tBankPtr> slot);
    sigc::connection onNumBanksChanged (sigc::slot<void, int> slot);

    Glib::ustring createPresetNameBasedOn (const Glib::ustring &oldName) const;
    void undoableAppendBank (UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid, bool autoSelect);
    void doAutoLoadSelectedPreset ();

    void importBank(InStream& stream, const Glib::ustring& x = "", const Glib::ustring& y = "", const Glib::ustring& fileName = "");

    void sanitizeBankClusterRelations(UNDO::Scope::tTransactionPtr transaction);

    Glib::ustring getDiffString(tPresetPtr preset1, tPresetPtr preset2);

  protected:
    void onTransactionAdded ();
    virtual tUpdateID onChange (uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

  private:
    PresetManager (const PresetManager &);
    PresetManager (PresetManager &);
    void operator= (const PresetManager &);
    void operator= (PresetManager &);

    void doSaveTask ();

    void reassignOrderNumbers ();

    void clearBanks (UNDO::Scope::tTransactionPtr transaction);
    void clearEditBuffer (UNDO::Scope::tTransactionPtr transaction);

    void onSelectedBankChanged ();
    void signalPresetManagerChanged ();

    typedef std::list<UNDO::Scope::tTransactionPtr> tTransactionList;
    void recurseIsoDepthTransactions (Writer &writer, const tTransactionList &isoDepth) const;

    Glib::ustring getCommaSeparated (const tTransactionList &list) const;

    Glib::ustring createPresetNameBasedOnString (const Glib::ustring &name) const;
    void loadMetadataAndSendEditBufferToLpc (UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> folder);
    void loadInitSound (UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> pmFolder);
    void loadBanks (UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> file);
    SaveResult saveMetadata (RefPtr<Gio::File> file);
    SaveResult saveInitSound (RefPtr<Gio::File> pmFolder);
    SaveResult saveBanks (RefPtr<Gio::File> file);
    Glib::ustring getBaseName (const ustring &basedOn) const;
    void deleteOldBanks (const Glib::RefPtr<Gio::File> pmFolder);

    void undoableSelectSibling (UNDO::Scope::tTransactionPtr transaction, int idx);

    void scheduleSave ();
    void scheduleAutoLoadSelectedPreset ();

    list<PresetManager::tSaveSubTask> createListOfSaveSubTasks ();
    void popSaveTaskAndRecurseSynchronously ();
    void popSaveTaskAndRecurseAsynchronously ();
    void recurseSaveAsynchronously ();

    void resolveCyclicAttachments(UNDO::Scope::tTransactionPtr transaction);
    void enforceClusterRuleOfOne();
    std::vector<PresetManager::tBankPtr> getClusterMasters();
    std::tuple<PresetManager::tBankPtr, PresetManager::tBankPtr> getChildrenOfMaster(Glib::ustring masterUUID);

    vector<tBankPtr> m_banks;
    shared_ptr<EditBuffer> m_editBuffer;
    shared_ptr<EditBuffer> m_initSound;

    Glib::ustring m_selectedBankUUID;

    typedef shared_ptr<RPCActionManager> tRPCActionManagerPtr;
    list<tRPCActionManagerPtr> m_actionManagers;

    friend class PresetManagerSerializer;
    friend class PresetManagerMetadataSerializer;
    friend class PresetBankOrderSerializer;
    friend class PresetManagerActions;

    Signal<void, tBankPtr> m_sigBankSelection;
    Signal<void, int> m_sigNumBanksChanged;

    Expiration m_saveJob;
    tUpdateID m_lastSavedMetdaDataUpdateID = 0;
    tUpdateID m_lastSavedInitSoundUpdateID = 0;

    Throttler m_autoLoadThrottler;
    RecursionGuard m_loading;

    list<tSaveSubTask> m_saveTasks;
    bool m_saveRequestDuringSave = false;
};
