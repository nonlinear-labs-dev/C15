#pragma once

#include "UndoableVector.h"

#include <memory>
#include <vector>
#include <string>

#include <http/UpdateDocumentContributor.h>
#include <presets/SearchQuery.h>
#include <serialization/PresetManagerSerializer.h>
#include <tools/ScopedGuard.h>
#include <tools/Throttler.h>
#include <tools/Uuid.h>
#include <tools/RecursionGuard.h>

class Bank;
class Preset;
class EditBuffer;
class Parameter;

class PresetManager : public ContentSection
{
  using SaveSubTask = function<SaveResult()>;

 public:
  PresetManager(UpdateDocumentContributor *parent);
  ~PresetManager() override;

  void init();

  // debug
  void stress(int numTransactions);
  void stressAllParams(int numParamChangedForEachParameter);
  void incAllParamsFine();
  void stressParam(UNDO::Transaction* transaction, Parameter* param);
  void stressBlocking(int numTransactions);
  void stressLoad(int numTransactions);

  // supported interfaces
  UpdateDocumentContributor::tUpdateID onChange(uint64_t flags = ChangeFlags::Generic) override;
  Glib::ustring getPrefix() const override;
  void handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  template <typename Mgr> Mgr &findActionManager()
  {
    for(auto &a : m_actionManagers)
      if(auto m = std::dynamic_pointer_cast<Mgr>(a))
        return *m.get();

    throw std::runtime_error("ActionManager does not exist in object");
  }

  // ???
  void doAutoLoadSelectedPreset();
  bool isLoading() const;
  std::shared_ptr<ScopedGuard::Lock> getLoadingLock();

  // accessors
  Bank *findBank(const Uuid &uuid) const;
  Preset *findPreset(const Uuid &uuid) const;
  Bank *findBankWithPreset(const Uuid &uuid) const;
  size_t getNumBanks() const;
  std::list<Bank *> getBanks() const;
  Bank *getBankAt(size_t idx) const;
  const Uuid &getSelectedBankUuid() const;
  Bank *getSelectedBank() const;
  EditBuffer *getEditBuffer() const;
  void forEachBank(std::function<void(Bank *b)> cb) const;

  // convenience
  void selectPreviousBank();
  void selectNextBank();
  void onPresetSelectionChanged();

  std::shared_ptr<ScopedGuard::Lock> lockLoading();

  // algorithms
  std::pair<double, double> calcDefaultBankPositionFor(const Bank *bank) const;
  size_t getBankPosition(const Uuid &uuid) const;

  // transactions
  Bank *addBank(UNDO::Transaction *transaction);
  Bank *addBank(UNDO::Transaction *transaction, std::unique_ptr<Bank> bank);
  void moveBank(UNDO::Transaction *transaction, const Bank *bankToMove, const Bank *moveBefore);
  void deleteBank(UNDO::Transaction *transaction, const Uuid &uuid);
  void selectBank(UNDO::Transaction *transaction, const Uuid &uuid);
  bool selectPreviousBank(UNDO::Transaction *transaction);
  bool selectNextBank(UNDO::Transaction *transaction);
  void sortBanks(UNDO::Transaction *transaction, const std::vector<Bank *> &banks);
  void storeInitSound(UNDO::Transaction *transaction);
  void resetInitSound(UNDO::Transaction *transaction);
  void loadInitSound(UNDO::Transaction *transaction);
  void clear(UNDO::Transaction *transaction);
  void setOrderNumber(UNDO::Transaction *transaction, const Uuid &bank, size_t targetPos);
  void sanitizeBankClusterRelations(UNDO::Transaction *transaction);
  void resolveCyclicAttachments(UNDO::Transaction *transaction);

  // algorithms
  Glib::ustring createPresetNameBasedOn(const Glib::ustring &basedOn) const;
  void searchPresets(Writer &writer, const Glib::ustring &q, const Glib::ustring &mode,
                     std::vector<SearchQuery::Fields> &&fieldsToSearch) const;

  // signals
  sigc::connection onBankSelection(sigc::slot<void> cb);
  sigc::connection onNumBanksChanged(sigc::slot<void, size_t> cb);

 private:
  void loadMetadataAndSendEditBufferToLpc(UNDO::Transaction *transaction, RefPtr<Gio::File> pmFolder);
  void loadInitSound(UNDO::Transaction *transaction, RefPtr<Gio::File> pmFolder);
  void loadBanks(UNDO::Transaction *transaction, RefPtr<Gio::File> pmFolder);
  Glib::ustring getBaseName(const ustring &basedOn) const;
  void scheduleAutoLoadSelectedPreset();

  list<PresetManager::SaveSubTask> createListOfSaveSubTasks();
  SaveResult saveMetadata(RefPtr<Gio::File> pmFolder);
  SaveResult saveInitSound(RefPtr<Gio::File> pmFolder);
  SaveResult saveBanks(RefPtr<Gio::File> pmFolder);

  void doSaveTask();
  void popSaveTaskAndRecurseSynchronously();
  void popSaveTaskAndRecurseAsynchronously();
  void recurseSaveAsynchronously();
  void scheduleSave();

  size_t getPreviousBankPosition() const;
  size_t getNextBankPosition() const;
  void selectBank(size_t idx);
  bool selectBank(UNDO::Transaction *transaction, size_t idx);
  void invalidateAllBanks();

  UndoableVector<Bank> m_banks;

  typedef shared_ptr<RPCActionManager> tRPCActionManagerPtr;
  std::list<tRPCActionManagerPtr> m_actionManagers;
  std::unique_ptr<EditBuffer> m_editBuffer;
  std::unique_ptr<Preset> m_initSound;

  ScopedGuard m_isLoading;
  Signal<void> m_sigBankSelection;
  Signal<void, size_t> m_sigNumBanksChanged;

  Throttler m_autoLoadThrottler;

  Expiration m_saveJob;
  tUpdateID m_lastSavedInitSoundUpdateID = 0;
  tUpdateID m_lastSavedMetaDataUpdateID = 0;

  list<SaveSubTask> m_saveTasks;
  bool m_saveRequestDuringSave = false;

  friend class PresetManagerSerializer;
};
