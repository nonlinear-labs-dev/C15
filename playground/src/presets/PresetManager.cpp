#include <Application.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/Settings.h>
#include <gio/gfile.h>
#include <giomm/fileenumerator.h>
#include <giomm/fileinfo.h>
#include <glibmm/regex.h>
#include <groups/ParameterGroup.h>
#include <libundo/undo/SwapData.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <Options.h>
#include <presets/BankActions.h>
#include <presets/EditBuffer.h>
#include <presets/EditBufferActions.h>
#include <presets/ParameterGroupSet.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <presets/PresetManagerActions.h>
#include <presets/SearchQuery.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <serialization/PresetManagerMetadataSerializer.h>
#include <serialization/PresetSerializer.h>
#include <xml/VersionAttribute.h>
#include <sigc++/functors/mem_fun.h>
#include <tools/FileSystem.h>
#include <tools/IntrusiveList.h>
#include <tools/PerformanceTimer.h>
#include <xml/Attribute.h>
#include <xml/XmlClass.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <iterator>
#include <sstream>
#include <string>
#include <presets/ClusterEnforcement.h>

constexpr static auto s_saveInterval = std::chrono::seconds(5);

PresetManager::PresetManager(UpdateDocumentContributor *parent) :
    ContentSection(parent),
    m_editBuffer(EditBuffer::createEditBuffer(this)),
    m_initSound(EditBuffer::createEditBuffer(this)),
    m_saveJob(bind(mem_fun(this, &PresetManager::doSaveTask))),
    m_autoLoadThrottler(std::chrono::milliseconds(500))
{
  m_actionManagers.emplace_back(new PresetManagerActions(*this));
  m_actionManagers.emplace_back(new BankActions(*this));
  m_actionManagers.emplace_back(new EditBufferActions(m_editBuffer));
}

PresetManager::~PresetManager()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);

  auto tasks = createListOfSaveSubTasks();

  for(auto &task : tasks)
  {
    while(task() == SaveResult::Again)
      ;
  }

  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

void PresetManager::init()
{
  load();
  getUndoScope().reset();
}

void PresetManager::reload()
{
  load();
  getUndoScope().reset();
}

void PresetManager::doAutoLoadSelectedPreset()
{
  if(auto lock = m_loading.lock())
  {
    FocusAndMode focusAndMode = Application::get().getHWUI()->getFocusAndMode();

    bool isPresetManagerActive = (focusAndMode.focus == UIFocus::Banks || focusAndMode.focus == UIFocus::Presets);
    bool isStoring = (focusAndMode.mode == UIMode::Store);
    bool isStoringPreset = isPresetManagerActive && isStoring;

    if(!isStoringPreset)
    {
      scheduleAutoLoadSelectedPreset();
    }
  }
}

void PresetManager::importBank(InStream& stream, const Glib::ustring& x, const Glib::ustring& y, const Glib::ustring& fileName)
{
  for(auto actionManager : m_actionManagers)
  {
    if(auto carstenPtr = dynamic_cast<BankActions*>(actionManager.get()))
    {
      carstenPtr->importBank(stream, x, y, fileName);
      break;
    }
  }
}

void PresetManager::scheduleAutoLoadSelectedPreset()
{
  m_autoLoadThrottler.doTask([ = ]()
  {
    if (auto b = getSelectedBank())
    {
      auto presetUUID = b->getSelectedPreset();
      auto eb = getEditBuffer();
      bool canOmitLoad = eb->getUUIDOfLastLoadedPreset() == presetUUID && !eb->isModified();

      if(!canOmitLoad)
      {
        if (auto p = b->getPreset (presetUUID))
        {
          if (auto currentUndo = getUndoScope().getUndoTransaction())
          {
            if (!currentUndo->isClosed())
            {
              eb->undoableLoad (currentUndo, p);
            }
            else
            {
              currentUndo->reopen();
              eb->undoableLoad (currentUndo, p);
              currentUndo->close();
            }
            return;
          }

          UNDO::Scope::tTransactionScopePtr scope = getUndoScope ().startTransaction (p->getUndoTransactionTitle ("Load"));
          eb->undoableLoad (scope->getTransaction(), p);
        }
      }
    }
  });
}

void PresetManager::undoableSelectBank(const Glib::ustring &uuid)
{
  UNDO::Scope::tTransactionScopePtr scope = getUndoScope().startContinuousTransaction(&m_selectedBankUUID, "Select bank '%0'",
      findBank(uuid)->getName(true));
  undoableSelectBank(scope->getTransaction(), uuid);
}

bool PresetManager::undoableSelectBank(UNDO::Scope::tTransactionPtr transaction, int idx)
{
  if(idx >= 0 && idx < getNumBanks())
  {
    undoableSelectBank(transaction, getBank(idx)->getUuid());
    return true;
  }
  return false;
}

void PresetManager::undoableSelectBank(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &uuid)
{
  auto swapData = UNDO::createSwapData(uuid);

  if(m_selectedBankUUID != uuid)
  {
    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      swapData->swapWith (m_selectedBankUUID);
      onChange ();
      m_sigBankSelection.send (getSelectedBank());
    });

    if(auto selBank = getSelectedBank())
    {
      selBank->undoableEnsurePresetSelection(transaction);
    }

    if(Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->get())
    {
      doAutoLoadSelectedPreset();
    }
  }
}

void PresetManager::undoableChangeBankOrder(UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid, moveDirection direction)
{
  if(auto bank = findBank(uuid))
  {
    if(int oldOrderNumber = calcOrderNumber(bank.get()))
    {
      int oldPos = oldOrderNumber - 1;
      int newPos = oldPos + ((direction == moveDirection::RightByOne) ? 1 : -1);

      if(newPos >= 0 && newPos < getNumBanks())
      {
        transaction->addSimpleCommand([ = ] (UNDO::Command::State)
        {
          auto a = getBank(oldPos);
          auto b = getBank(newPos);
          m_banks[newPos] = a;
          m_banks[oldPos] = b;
          reassignOrderNumbers();
        });
      }
    }
  }
}

void PresetManager::undoableSetOrderNumber(UNDO::Scope::tTransactionPtr transaction, tBankPtr bank, int newOrderNumber)
{
  int newPos = std::min<int>(newOrderNumber - 1, getNumBanks() - 1);
  newPos = std::max(newPos, 0);

  auto swapData = UNDO::createSwapData(newPos);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State)
  {
    auto it = std::find(m_banks.begin(), m_banks.end(), bank);
    int pos = std::distance(m_banks.begin(), it);
    swapData->swapWith(pos);
    m_banks.erase(it);
    m_banks.emplace(m_banks.begin() + pos, bank);
    reassignOrderNumbers();
  });
}

void PresetManager::undoableSelectNext()
{
  auto selected = getSelectedBank();

  if(auto num = getNumBanks())
  {
    auto found = false;

    for(auto i = 0u; i < num; i++)
    {
      if(found)
      {
        undoableSelectBank(getBank(i)->getUuid());
        return;
      }

      found = getBank(i) == selected;
    }

    if(!found)
    {
      undoableSelectBank(getBank(0)->getUuid());
    }
  }
}

void PresetManager::undoableSelectPrevious()
{
  tBankPtr theOneBefore;

  auto selected = getSelectedBank();

  if(auto num = getNumBanks())
  {
    for(auto i = 0u; i < num; i++)
    {
      auto currentBank = getBank(i);

      if(currentBank == selected)
      {
        if(theOneBefore)
        {
          undoableSelectBank(theOneBefore->getUuid());
        }
        return;
      }

      theOneBefore = currentBank;
    }

    undoableSelectBank(getBank(0)->getUuid());
  }
}

PresetManager::tBankPtr PresetManager::addBank(UNDO::Scope::tTransactionPtr transaction, bool autoSelect)
{
  vector<tBankPtr> banks = m_banks;
  tBankPtr newBank(new PresetBank(this));
  banks.insert(banks.end(), newBank);

  auto swapData = UNDO::createSwapData(banks);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    swapData->swapWith (m_banks);
    reassignOrderNumbers();
    signalPresetManagerChanged();
  });

  if(autoSelect)
  {
    undoableSelectBank(transaction, newBank->getUuid());
  }

  return newBank;
}

PresetManager::tBankPtr PresetManager::addBank(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &x, const Glib::ustring &y)
{
  auto newBank = addBank(transaction, true);
  newBank->undoableSetPosition(transaction, x, y);
  return newBank;
}

void PresetManager::undoableAppendBank(UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid, bool autoSelect)
{
  auto newBank = addBank(transaction, autoSelect);
  newBank->setUuid(transaction, uuid);
}

void PresetManager::undoableDeleteBank(UNDO::Scope::tTransactionPtr transaction, const tBankPtr bank)
{
  int idx = calcBankIndex(bank.get());

  ClusterEnforcement enforcer;
  enforcer.sanitizeBankThatWillBeDeleted(transaction, bank);

  if(idx >= 0)
  {
    tBankPtr swappedBank;
    auto swap = UNDO::createSwapData(swappedBank);

    if(bank->getUuid() == m_selectedBankUUID)
      undoableSelectSibling(transaction, idx);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      swap->swapWith (m_banks[idx]);
      m_banks.erase (m_banks.begin () + idx);
      reassignOrderNumbers();
      signalPresetManagerChanged();
    }, [ = ] (UNDO::Command::State) mutable
    {
      m_banks.insert (m_banks.begin() + idx, tBankPtr());
      swap->swapWith (m_banks[idx]);
      m_banks[idx]->onRestore();
      reassignOrderNumbers();
      signalPresetManagerChanged();
    });

    getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
  }
}

void PresetManager::undoableSelectSibling(UNDO::Scope::tTransactionPtr transaction, int idx)
{
  if(!undoableSelectBank(transaction, idx + 1))
    if(!undoableSelectBank(transaction, idx - 1))
      undoableSelectBank(transaction, "");
}

void PresetManager::undoableDeleteSelectedBank(UNDO::Scope::tTransactionPtr transaction)
{
  if(tBankPtr selBank = getSelectedBank())
  {
    undoableDeleteBank(transaction, selBank);
  }
}

void PresetManager::undoableClear()
{
  UNDO::Scope::tTransactionScopePtr scope = getUndoScope().startTransaction("Clear preset manager");
  UNDO::Scope::tTransactionPtr transaction = scope->getTransaction();
  undoableClear(transaction);
}

void PresetManager::undoableClear(UNDO::Scope::tTransactionPtr transaction)
{
  clearEditBuffer(transaction);
  clearBanks(transaction);
}

void PresetManager::clearBanks(UNDO::Scope::tTransactionPtr transaction)
{
  auto swapData = UNDO::createSwapData(vector<tBankPtr>());

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    swapData->swapWith (m_banks);
    reassignOrderNumbers();
    signalPresetManagerChanged();
  });
}

void PresetManager::clearEditBuffer(UNDO::Scope::tTransactionPtr transaction)
{
  getEditBuffer()->undoableClear(transaction);
}

shared_ptr<EditBuffer> PresetManager::getEditBuffer()
{
  return m_editBuffer;
}

size_t PresetManager::getNumBanks() const
{
  return m_banks.size();
}

void PresetManager::signalPresetManagerChanged()
{
  onChange();
}

PresetManager::tBankPtr PresetManager::getBank(int pos)
{
  if(pos < 0 || pos >= (int) getNumBanks())
    return NULL;

  return m_banks[pos];
}

const PresetManager::tBankPtr PresetManager::getBank(int pos) const
{
  if(pos < 0 || pos >= (int) getNumBanks())
    return NULL;

  return m_banks[pos];
}

void PresetManager::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("preset-manager", Attribute("changed", changed), Attribute("file-version", VersionAttribute::getCurrentFileVersion()),
      [&]()
      {
        if (changed)
        {
          m_editBuffer->writeDocument (writer, knownRevision);

          writer.writeTag ("banks", Attribute ("selected-bank", m_selectedBankUUID), [&]()
              {
                for (tBankPtr bank : m_banks)
                bank->writeDocument (writer, knownRevision);
              });
        }
      });
}

void PresetManager::searchPresets(Writer &writer, const Glib::ustring &q, const Glib::ustring &mode,
                                  std::vector<presetInfoSearchFields> &&fieldsToSearch) const
{
  SearchQuery query(q, mode, std::move(fieldsToSearch));

  writer.writeTag("preset-manager", [&]()
  {
    writer.writeTag ("banks", [&]()
        {
          for (tBankPtr bank : m_banks)
          {
            bank->searchPresets (writer, query);
          }
        });
  });
}

void PresetManager::recurseIsoDepthTransactions(Writer &writer, const tTransactionList &isoDepth) const
{
  tTransactionList children;

  writer.writeTag("div", XmlClass("undo-tree-layer"), [&]()
  {
    for (UNDO::Scope::tTransactionPtr t : isoDepth)
    {
      tTransactionList myChildren;
      t->addChildren (myChildren);

      bool isSelectedLayer = getUndoScope().getUndoTransaction() == t;
      XmlClass classAttribute (isSelectedLayer ? "undo-tree-entry selected" : "undo-tree-entry");

      std::stringstream undoCommand;
      undoCommand << "$.get('/preset-manager/undo-jump?target=" << t.get() << "');";

      writer.writeTextElement ("div", t->getName(), classAttribute,
          Attribute ("onclick", undoCommand.str()), Attribute ("id", t.get()),
          Attribute ("children", getCommaSeparated (myChildren)));

      children.insert (children.end(), myChildren.begin(), myChildren.end());
    }

    writer.writeTextElement ("canvas", "", XmlClass ("undo-tree-entry-connections"),
        Attribute ("ins", getCommaSeparated (isoDepth)),
        Attribute ("width", 1000), Attribute ("height", 1000));
  });

  if(!children.empty())
    recurseIsoDepthTransactions(writer, children);
}

Glib::ustring PresetManager::getCommaSeparated(const tTransactionList &list) const
{
  std::stringstream str;

  for(UNDO::Scope::tTransactionPtr t : list)
  {
    if(str.tellp())
      str << ",";

    str << t.get();
  }

  return str.str();
}

Glib::ustring PresetManager::getPrefix() const
{
  return "presets";
}

void PresetManager::handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path)
{
  ContentSection::handleHTTPRequest(request, path);

  for(auto actionManager : m_actionManagers)
    if(actionManager->matches(path))
      if(actionManager->handleRequest(path, request))
        return;

  DebugLevel::warning("could not handle request", path);
}

PresetManager::tBankPtr PresetManager::findBankWithPreset(const Glib::ustring &presetUUID)
{
  for(auto bank : m_banks)
  {
    if(bank->containsPreset(presetUUID))
      return bank;
  }

  return NULL;
}

PresetManager::tPresetPtr PresetManager::findPreset(const Glib::ustring &presetUUID)
{
  for(auto bank : m_banks)
    if(auto p = bank->getPreset(presetUUID))
      return p;

  return NULL;
}

PresetManager::tBankPtr PresetManager::getSelectedBank() const
{
  return findBank(m_selectedBankUUID);
}

int PresetManager::calcOrderNumber(const PresetBank *bank) const
{
  return calcBankIndex(bank) + 1;
}

int PresetManager::calcBankIndex(const PresetBank *bank) const
{
  for(int i = 0; i < getNumBanks(); i++)
    if(m_banks[i].get() == bank)
      return i;

  return -1;
}

PresetManager::tBankPtr PresetManager::findBank(const Glib::ustring &uuid) const
{
  for(auto bank : m_banks)
  {
    if(bank->getUuid() == uuid)
      return bank;
  }

  return NULL;
}

bool PresetManager::isLoading() const
{
  return m_loading.isLocked();
}

void PresetManager::load()
{
  if(auto lock = m_loading.lock())
  {
    PerformanceTimer timer(__PRETTY_FUNCTION__);

    auto scope = getUndoScope().startTrashTransaction();
    auto transaction = scope->getTransaction();

    getEditBuffer()->undoableSelectParameter(transaction, "0");

    auto path = Application::get().getOptions()->getPresetManagerPath();

    DEBUG_TRACE("Loading presets at ", path);
    auto file = Gio::File::create_for_path(path);

    if(file->query_exists())
    {
      DEBUG_TRACE("Loading presetmanager at", path);
      loadMetadataAndSendEditBufferToLpc(transaction, file);
      loadInitSound(transaction, file);
      loadBanks(transaction, file);
      deleteOldBanks(file);
    }
    else
    {
      getEditBuffer()->sendToLPC();
    }

    m_sigBankSelection.send(getSelectedBank());

    auto hwui = Application::get().getHWUI();
    hwui->getPanelUnit().getEditPanel().getBoled().setupFocusAndMode(hwui->getFocusAndMode());
    hwui->getBaseUnit().getPlayPanel().getSOLED().resetSplash();
    onChange();
    m_lastSavedMetdaDataUpdateID = getUpdateIDOfLastChange();
  }
}

void PresetManager::doSaveTask()
{
  PerformanceTimer timer(__PRETTY_FUNCTION__, 0);

  if(!m_saveTasks.empty())
  {
    auto &firstTask = m_saveTasks.front();

    switch(firstTask())
    {
      case SaveResult::Nothing:
        popSaveTaskAndRecurseSynchronously();
        return;

      case SaveResult::Finished:
        popSaveTaskAndRecurseAsynchronously();
        break;

      case SaveResult::Again:
        recurseSaveAsynchronously();
        return;
    }
  }
  else if(m_saveRequestDuringSave)
  {
    m_saveRequestDuringSave = false;
    scheduleSave();
  }
}

void PresetManager::popSaveTaskAndRecurseSynchronously()
{
  m_saveTasks.pop_front();
  doSaveTask();
}

void PresetManager::popSaveTaskAndRecurseAsynchronously()
{
  m_saveTasks.pop_front();
  recurseSaveAsynchronously();
}

void PresetManager::recurseSaveAsynchronously()
{
  m_saveJob.refresh(std::chrono::milliseconds(5));
}

void PresetManager::scheduleSave()
{
  if(!m_saveJob.isPending())
  {
    m_saveTasks = createListOfSaveSubTasks();
    m_saveJob.refresh(s_saveInterval);
  }
  else
  {
    m_saveRequestDuringSave = true;
  }
}

list<PresetManager::tSaveSubTask> PresetManager::createListOfSaveSubTasks()
{
  auto path = Application::get().getOptions()->getPresetManagerPath();
  auto file = Gio::File::create_for_path(path);
  g_file_make_directory_with_parents(file->gobj(), nullptr, nullptr);

  return
  {
    bind (&PresetManager::saveMetadata, this, file),
    bind (&PresetManager::saveInitSound, this, file),
    bind (&PresetManager::saveBanks, this, file)
  };
}

SaveResult PresetManager::saveMetadata(RefPtr<Gio::File> pmFolder)
{
  if(m_lastSavedMetdaDataUpdateID != getUpdateIDOfLastChange())
  {
    PresetManagerMetadataSerializer serializer(*this);
    serializer.write(pmFolder, ".metadata");
    m_lastSavedMetdaDataUpdateID = getUpdateIDOfLastChange();
    return SaveResult::Finished;
  }

  return SaveResult::Nothing;
}

void PresetManager::loadMetadataAndSendEditBufferToLpc(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> pmFolder)
{
  DEBUG_TRACE("loadMetadata", pmFolder->get_uri());
  SplashLayout::addStatus("Loading Edit Buffer");
  Serializer::read<PresetManagerMetadataSerializer>(transaction, pmFolder, ".metadata", std::ref(*this));
  getEditBuffer()->sendToLPC();
}

SaveResult PresetManager::saveInitSound(RefPtr<Gio::File> pmFolder)
{
  if(m_lastSavedInitSoundUpdateID != m_initSound->getUpdateIDOfLastChange())
  {
    PresetSerializer serializer(m_initSound.get(), true);
    serializer.write(pmFolder, ".initSound");
    m_lastSavedInitSoundUpdateID = m_initSound->getUpdateIDOfLastChange();
    return SaveResult::Finished;
  }
  return SaveResult::Nothing;
}

void PresetManager::loadInitSound(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> pmFolder)
{
  DEBUG_TRACE("loadInitSound", pmFolder->get_uri());
  SplashLayout::addStatus("Loading Init Sound");

  Serializer::read<PresetSerializer>(transaction, pmFolder, ".initSound", m_initSound.get(), true);

  m_editBuffer->undoableSetDefaultValues(transaction, m_initSound.get());
  m_lastSavedInitSoundUpdateID = getUpdateIDOfLastChange();
}

SaveResult PresetManager::saveBanks(RefPtr<Gio::File> pmFolder)
{
  for(auto b : m_banks)
  {
    auto bankFolder = pmFolder->get_child((Glib::ustring) b->getUuid());
    g_file_make_directory_with_parents(bankFolder->gobj(), nullptr, nullptr);

    switch(b->save(bankFolder))
    {
      case SaveResult::Again:
        return SaveResult::Again;

      case SaveResult::Finished:
        return SaveResult::Again;

      case SaveResult::Nothing:
        break; // try next bank
    }
  }

  return SaveResult::Nothing;
}

void PresetManager::deleteOldBanks(const RefPtr<Gio::File> pmFolder)
{
  RefPtr < Gio::FileEnumerator > enumerator = pmFolder->enumerate_children();

  while(auto file = enumerator->next_file())
  {
    if(file->get_file_type() == Gio::FILE_TYPE_DIRECTORY)
    {
      auto uuid = file->get_name();

      if(FileSystem::isNameAUUID(uuid))
      {
        if(!findBank(uuid))
        {
          DEBUG_TRACE("bank with uuid: ", uuid, " found...");
          FileSystem::deleteFolder(pmFolder->get_child(uuid));
          DEBUG_TRACE("...and deleted");
        }
      }
    }
  }
}

void PresetManager::loadBanks(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> pmFolder)
{
  DEBUG_TRACE("loadBanks", pmFolder->get_uri());
  SplashLayout::addStatus("Loading Banks");

  list<std::shared_ptr<PresetBank>> toDelete;

  int numBank = 1;
  int numBanks = m_banks.size();

  for(auto b : m_banks)
  {
    DEBUG_TRACE("loadBanks, bank:", b->getUuid());
    auto bankFolder = pmFolder->get_child((Glib::ustring) b->getUuid());

    try
    {
      b->loadSync(transaction, bankFolder, numBank++, numBanks);
    }
    catch(...)
    {
      DEBUG_TRACE("could not load bank", b->getUuid());
      toDelete.push_back(b);
    }
  }
  auto banksDeleted = 0;
  for(auto b : toDelete)
  {
    undoableDeleteBank(transaction, b);
    banksDeleted++;
  }
  DEBUG_TRACE("Deleted ", banksDeleted, " Banks in loadBanks!");
}

void PresetManager::onTransactionAdded()
{
  onChange();
}

UpdateDocumentContributor::tUpdateID PresetManager::onChange(uint64_t flags)
{
  scheduleSave();
  return UpdateDocumentContributor::onChange(flags);
}

sigc::connection PresetManager::onBankSelection(sigc::slot<void, tBankPtr> slot)
{
  return m_sigBankSelection.connectAndInit(slot, getSelectedBank());
}

sigc::connection PresetManager::onNumBanksChanged(sigc::slot<void, int> slot)
{
  return m_sigNumBanksChanged.connectAndInit(slot, getNumBanks());
}

Glib::ustring PresetManager::createPresetNameBasedOn(const Glib::ustring &basedOn) const
{

  const Glib::ustring base = getBaseName(basedOn);

  if(base.empty())
    return "New preset";

  int highestPostfix = 0;

  for(auto b : m_banks)
  {
    highestPostfix = std::max(highestPostfix, b->getHighestIncrementForBaseName(base));
  }

  if(highestPostfix == 0)
    return base;

  const int newPostfix = highestPostfix + 1;
  return base + " " + to_string(newPostfix);
}

void PresetManager::reassignOrderNumbers()
{
  for(tBankPtr b : m_banks)
    b->onChange();

  m_sigNumBanksChanged.send(getNumBanks());
}

Glib::ustring PresetManager::getBaseName(const ustring &basedOn) const
{
  auto regex = Glib::Regex::create("^(.*)\\s([0-9]+)$");
  Glib::MatchInfo matchInfo;

  if(regex->match(basedOn, matchInfo) && matchInfo.get_match_count() > 1)
  {
    for(int i = 0; i < matchInfo.get_match_count(); i++)
    {
      DebugLevel::info(matchInfo.fetch(i));
    }

    return matchInfo.fetch(1);
  }

  return basedOn;
}

auto PresetManager::getBanks() const -> const vector<tBankPtr> &
{
  return m_banks;
}

void PresetManager::undoableStoreInitSound(UNDO::Scope::tTransactionPtr transaction)
{
  m_initSound->copyFrom(transaction, m_editBuffer.get(), true);
  m_editBuffer->undoableSetDefaultValues(transaction, m_initSound.get());
}

void PresetManager::undoableLoadInitSound(UNDO::Scope::tTransactionPtr transaction)
{
  m_editBuffer->copyFrom(transaction, m_initSound.get(), true);
}

void PresetManager::undoableResetInitSound(UNDO::Scope::tTransactionPtr transaction)
{
  for(auto group : m_initSound->getParameterGroups())
    group->undoableClear(transaction);

  m_editBuffer->undoableSetDefaultValues(transaction, m_initSound.get());
}

PresetManager::tPresetPtr PresetManager::getSelectedPreset() const
{
  if(auto b = getSelectedBank())
  {
    auto uuid = b->getSelectedPreset();
    return b->getPreset(uuid);
  }
  return nullptr;
}

void PresetManager::sanitizeBankClusterRelations(UNDO::Scope::tTransactionPtr transaction)
{
  resolveCyclicAttachments(transaction);
  ClusterEnforcement enforcer;
  enforcer.enforceClusterRuleOfOne(transaction);
}

void PresetManager::resolveCyclicAttachments(UNDO::Scope::tTransactionPtr transaction)
{
  auto presetManager = Application::get().getPresetManager();
  auto allBanks = presetManager->getBanks();
  for(auto bank : allBanks)
  {
    bank->resolveCyclicAttachments(transaction);
  }
}

void PresetManager::stress(int numTransactions)
{
  Glib::MainContext::get_default()->signal_timeout().connect_once([=]()
  {
    int parameterId = g_random_int_range(0, 200);

    {
      auto transactionScope = getUndoScope().startTransaction("Stressing Undo System");
      m_editBuffer->undoableSelectParameter(transactionScope->getTransaction(), to_string(parameterId));

      if(auto p = m_editBuffer->getSelected())
      {
        p->stepCPFromHwui (transactionScope->getTransaction(), g_random_boolean() ? -1 : 1, ButtonModifiers());
      }
    }

    if(numTransactions % 20 == 0)
    {
      int numUndos = g_random_int_range(1, 5);

      for(int i = 0; i < numUndos; i++)
      {
        getUndoScope().undo();
      }
    }

    if(numTransactions > 0)
    {
      stress(numTransactions - 1);
    }

  }, 20);
}

Glib::ustring PresetManager::getDiffString(tPresetPtr preset1, tPresetPtr preset2)
{
  Glib::ustring out;

  auto parameters1 = preset1->getParametersSortedById();
  auto parameters2 = preset2->getParametersSortedById();

  for(auto it_m1 = parameters1.cbegin(), end_m1 = parameters1.cend(), it_m2 = parameters2.cbegin(), end_m2 = parameters2.cend();
      it_m1 != end_m1 || it_m2 != end_m2;)
  {
    if(it_m1 != end_m1 && it_m2 != end_m2)
    {
      if(it_m1->second->getDisplayString() != it_m2->second->getDisplayString())
      {
        out += Glib::ustring(it_m1->second->getParentGroup()->getLongName());
        out += ",";
        out += Glib::ustring(it_m1->second->getLongName());
        out += ",";
        out += Glib::ustring(it_m1->second->getDisplayString());
        out += ",";
        out += Glib::ustring(it_m2->second->getDisplayString());
        out += ",";
      }
      ++it_m1;
      ++it_m2;
    }
  }

  return out;
}
