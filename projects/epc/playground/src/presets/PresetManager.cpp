#include <presets/PresetManager.h>
#include "Bank.h"
#include "libundo/undo/SwapData.h"
#include "libundo/undo/Transaction.h"
#include <algorithm>
#include <tools/PerformanceTimer.h>
#include <Application.h>
#include <Options.h>
#include <serialization/PresetManagerMetadataSerializer.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <presets/BankActions.h>
#include <presets/ClusterEnforcement.h>
#include <presets/EditBuffer.h>
#include <presets/EditBufferActions.h>
#include <presets/PresetManagerActions.h>
#include <presets/Preset.h>
#include <xml/VersionAttribute.h>
#include <proxies/hwui/HWUI.h>
#include <serialization/PresetSerializer.h>
#include <device-settings/DirectLoadSetting.h>
#include <device-settings/Settings.h>
#include <groups/ParameterGroup.h>
#include <glibmm.h>
#include <giomm/file.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/SelectVoiceGroupLayout.h>
#include <tools/StringTools.h>
#include <presets/PresetPartSelection.h>
#include <parameter_declarations.h>
#include <presets/PresetParameter.h>
#include <set>

constexpr static auto s_saveInterval = std::chrono::seconds(5);

PresetManager::PresetManager(UpdateDocumentContributor *parent, bool readOnly)
    : ContentSection(parent)
    , m_banks(*this, nullptr)
    , m_editBuffer(std::make_unique<EditBuffer>(this))
    , m_initSound(std::make_unique<Preset>(this))
    , m_saveJob(std::bind(&PresetManager::doSaveTask, this))
    , m_readOnly(readOnly)
{
  m_actionManagers.emplace_back(new PresetManagerActions(*this));
  m_actionManagers.emplace_back(new BankActions(*this));
  m_actionManagers.emplace_back(new EditBufferActions(m_editBuffer.get()));
  onRestoreHappened([&]() { invalidate(); });
}

PresetManager::~PresetManager()
{
  if(!m_readOnly)
  {
    auto tasks = createListOfSaveSubTasks();
    for(auto &task : tasks)
    {
      while(task() == SaveResult::Again)
        ;
    }
  }
}

void PresetManager::init()
{
  PerformanceTimer timer(__PRETTY_FUNCTION__);

  auto loadingLock = m_isLoading.lock();

  auto scope = getUndoScope().startTrashTransaction();
  auto transaction = scope->getTransaction();
  auto path = Application::get().getOptions()->getPresetManagerPath();
  auto file = Gio::File::create_for_path(path);

  clear(transaction);

  if(file->query_exists())
  {
    nltools::Log::notify("Loading presetmanager at", path);
    loadMetadataAndSendEditBufferToPlaycontroller(transaction, file);
    loadInitSound(transaction, file);
    loadBanks(transaction, file);
    fixMissingPresetSelections(transaction);
  }

  auto hwui = Application::get().getHWUI();
  hwui->getPanelUnit().getEditPanel().getBoled().setupFocusAndMode(hwui->getFocusAndMode());
  hwui->getBaseUnit().getPlayPanel().getSOLED().resetSplash();
  m_sigLoadHappened.send();
  onChange();
}

void PresetManager::invalidate()
{
  onChange(ChangeFlags::Generic);
}

Glib::ustring PresetManager::getPrefix() const
{
  return "presets";
}

UpdateDocumentContributor::tUpdateID PresetManager::onChange(uint64_t flags)
{
  scheduleSave();
  auto ret = UpdateDocumentContributor::onChange(flags);
  m_sigNumBanksChanged.send(getNumBanks());
  m_sigBankSelection.send(getSelectedBankUuid());
  return ret;
}

void PresetManager::handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path)
{
  ContentSection::handleHTTPRequest(request, path);

  for(auto &actionManager : m_actionManagers)
    if(actionManager->matches(path))
      if(actionManager->handleRequest(path, request))
        return;

  DebugLevel::warning("could not handle request", path);
}

std::list<PresetManager::SaveSubTask> PresetManager::createListOfSaveSubTasks()
{
  auto path = Application::get().getOptions()->getPresetManagerPath();
  auto file = Gio::File::create_for_path(path);
  g_file_make_directory_with_parents(file->gobj(), nullptr, nullptr);

  return { std::bind(&PresetManager::saveMetadata, this, file), std::bind(&PresetManager::saveInitSound, this, file),
           std::bind(&PresetManager::saveBanks, this, file) };
}

SaveResult PresetManager::saveMetadata(Glib::RefPtr<Gio::File> pmFolder)
{
  if(m_lastSavedMetaDataUpdateID != getUpdateIDOfLastChange())
  {
    PerformanceTimer timer(__PRETTY_FUNCTION__);
    PresetManagerMetadataSerializer serializer(this);
    serializer.write(pmFolder, ".metadata");
    m_lastSavedMetaDataUpdateID = getUpdateIDOfLastChange();
    return SaveResult::Finished;
  }

  return SaveResult::Nothing;
}

SaveResult PresetManager::saveInitSound(Glib::RefPtr<Gio::File> pmFolder)
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

void PresetManager::doSaveTask()
{
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
  if(!m_saveJob.isPending() && !m_readOnly)
  {
    m_saveTasks = createListOfSaveSubTasks();
    m_saveJob.refresh(s_saveInterval, Glib::PRIORITY_LOW);
  }
  else
  {
    m_saveRequestDuringSave = true;
  }
}

SaveResult PresetManager::saveBanks(Glib::RefPtr<Gio::File> pmFolder)
{
  for(auto &b : m_banks.getElements())
  {
    auto bankFolder = pmFolder->get_child(static_cast<Glib::ustring>(b->getUuid().raw()));
    g_file_make_directory_with_parents(bankFolder->gobj(), nullptr, nullptr);

    switch(b->save(bankFolder))
    {
      case SaveResult::Again:
        return SaveResult::Again;

      case SaveResult::Finished:
        return SaveResult::Again;

      case SaveResult::Nothing:
        break;  // try next bank
    }
  }

  return SaveResult::Nothing;
}

void PresetManager::undoableLoadSelectedPreset(UNDO::Transaction *currentTransactionPtr)
{
  if(auto lock = m_isLoading.lock())
  {
    auto hwui = Application::get().getHWUI();
    FocusAndMode focusAndMode = hwui->getFocusAndMode();

    bool isPresetManagerActive = (focusAndMode.focus == UIFocus::Banks || focusAndMode.focus == UIFocus::Presets);
    bool isStoring = (focusAndMode.mode == UIMode::Store);
    bool isStoringPreset = isPresetManagerActive && isStoring;

    if(!isStoringPreset)
    {
      undoableLoadSelectedPresetAccordingToLoadType(currentTransactionPtr);
    }
  }
}

bool PresetManager::isLoading() const
{
  return m_isLoading.isLocked();
}

std::shared_ptr<ScopedGuard::Lock> PresetManager::getLoadingLock()
{
  return m_isLoading.lock();
}

void PresetManager::loadMetadataAndSendEditBufferToPlaycontroller(UNDO::Transaction *transaction,
                                                                  const Glib::RefPtr<Gio::File> &pmFolder)
{
  DebugLevel::gassy("loadMetadata", pmFolder->get_uri());
  SplashLayout::addStatus("Loading Edit Buffer");
  Serializer::read<PresetManagerMetadataSerializer>(transaction, pmFolder, ".metadata", this);
  m_editBuffer->sendToAudioEngine();
}

void PresetManager::loadInitSound(UNDO::Transaction *transaction, const Glib::RefPtr<Gio::File> &pmFolder)
{
  DebugLevel::gassy("loadInitSound", pmFolder->get_uri());
  SplashLayout::addStatus("Loading Init Sound");

  Serializer::read<PresetSerializer>(transaction, pmFolder, ".initSound", m_initSound.get(), true);

  m_editBuffer->undoableSetDefaultValues(transaction, m_initSound.get());
  m_lastSavedInitSoundUpdateID = getUpdateIDOfLastChange();
}

void PresetManager::loadBanks(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> pmFolder)
{
  DebugLevel::gassy("loadBanks", pmFolder->get_uri());
  SplashLayout::addStatus("Loading Banks");

  int numBanks = static_cast<int>(m_banks.size());

  m_banks.forEach([&, currentBank = 1](Bank *bank) mutable {
    DebugLevel::gassy("loadBanks, bank:", bank->getUuid().raw());
    auto bankFolder = pmFolder->get_child(bank->getUuid().raw());
    bank->load(transaction, bankFolder, currentBank++, numBanks);
  });
}

void PresetManager::fixMissingPresetSelections(UNDO::Transaction *transaction)
{
  m_banks.forEach([&](auto bank) { bank->ensurePresetSelection(transaction); });
}

Bank *PresetManager::findBank(const Uuid &uuid) const
{
  return m_banks.find(uuid);
}

Preset *PresetManager::findPreset(const Uuid &uuid) const
{
  for(auto &e : m_banks.getElements())
    if(auto p = e->findPreset(uuid))
      return p;

  return nullptr;
}

Bank *PresetManager::findBankWithPreset(const Uuid &uuid) const
{
  if(auto p = findPreset(uuid))
    return dynamic_cast<Bank *>(p->getParent());

  return nullptr;
}

size_t PresetManager::getNumBanks() const
{
  return m_banks.size();
}

std::list<Bank *> PresetManager::getBanks() const
{
  std::list<Bank *> ret;
  m_banks.forEach([&](auto bank) { ret.push_back(bank); });
  return ret;
}

Bank *PresetManager::getBankAt(size_t idx) const
{
  if(!m_banks.empty())
    return m_banks.at(idx);
  return nullptr;
}

const Uuid &PresetManager::getSelectedBankUuid() const
{
  return m_banks.getSelectedUuid();
}

Bank *PresetManager::getSelectedBank() const
{
  return m_banks.getSelected();
}

EditBuffer *PresetManager::getEditBuffer() const
{
  return m_editBuffer.get();
}

void PresetManager::forEachBank(std::function<void(Bank *)> cb) const
{
  m_banks.forEach(cb);
}

std::shared_ptr<ScopedGuard::Lock> PresetManager::lockLoading()
{
  return m_isLoading.lock();
}

bool PresetManager::selectPreviousBank(UNDO::Transaction *transaction)
{
  return selectBank(transaction, getPreviousBankPosition());
}

bool PresetManager::selectNextBank(UNDO::Transaction *transaction)
{
  return selectBank(transaction, getNextBankPosition());
}

bool PresetManager::selectBank(UNDO::Transaction *transaction, size_t idx)
{
  if(idx < getNumBanks())
  {
    auto bank = getBankAt(idx);
    selectBank(transaction, bank->getUuid());
    return true;
  }
  return false;
}

size_t PresetManager::getNextBankPosition() const
{
  return m_banks.getNextPosition(getSelectedBankUuid());
}

size_t PresetManager::getPreviousBankPosition() const
{
  return m_banks.getPreviousPosition(getSelectedBankUuid());
}

Bank *PresetManager::addBank(UNDO::Transaction *transaction)
{
  return m_banks.append(transaction, std::make_unique<Bank>(this));
}

Bank *PresetManager::addBank(UNDO::Transaction *transaction, std::unique_ptr<Bank> bank)
{
  return m_banks.append(transaction, std::move(bank));
}

void PresetManager::moveBank(UNDO::Transaction *transaction, const Bank *bankToMove, const Bank *moveBefore)
{
  m_banks.move(transaction, bankToMove, moveBefore);
}

void PresetManager::deleteBank(UNDO::Transaction *transaction, const Uuid &uuid)
{
  handleDockingOnBankDelete(transaction, uuid);
  m_banks.remove(transaction, uuid);
}

bool handleMaster(Bank *master, Bank *bottom, Bank *right, UNDO::Transaction *transaction,
                  Bank::AttachmentDirection dir)
{
  if(master)
  {
    if(bottom)
    {
      bottom->setAttachedToBank(transaction, master->getUuid());
      bottom->setAttachedDirection(transaction, std::to_string(static_cast<int>(dir)));

      if(right)
      {
        right->setAttachedToBank(transaction, bottom->getUuid());
        right->setAttachedDirection(transaction, std::to_string(static_cast<int>(Bank::AttachmentDirection::left)));
      }
      return true;
    }
    else if(right)
    {
      right->setAttachedToBank(transaction, master->getUuid());
      right->setAttachedDirection(transaction, std::to_string(static_cast<int>(Bank::AttachmentDirection::left)));
      return true;
    }
  }
  return false;
}

void PresetManager::handleDockingOnBankDelete(UNDO::Transaction *transaction, const Uuid &uuid)
{
  auto bankToDelete = m_banks.find(uuid);
  if(bankToDelete)
  {
    auto slaveBottom = bankToDelete->getSlaveBottom();
    auto slaveRight = bankToDelete->getSlaveRight();
    auto masterTop = bankToDelete->getMasterTop();
    auto masterLeft = bankToDelete->getMasterLeft();

    if(handleMaster(masterTop, slaveBottom, slaveRight, transaction, Bank::AttachmentDirection::top))
    {
    }
    else if(handleMaster(masterLeft, slaveBottom, slaveRight, transaction, Bank::AttachmentDirection::left))
    {
    }
    else if(slaveBottom && slaveRight)
    {
      slaveRight->setAttachedToBank(transaction, slaveBottom->getUuid());
      slaveBottom->setX(transaction, bankToDelete->getX());
      slaveBottom->setY(transaction, bankToDelete->getY());
    }
    else if(slaveBottom)
    {
      slaveBottom->setX(transaction, bankToDelete->getX());
      slaveBottom->setY(transaction, bankToDelete->getY());
    }
    else if(slaveRight)
    {
      slaveRight->setX(transaction, bankToDelete->getX());
      slaveRight->setY(transaction, bankToDelete->getY());
    }
  }
}

void PresetManager::selectBank(UNDO::Transaction *transaction, const Uuid &uuid)
{
  m_banks.select(transaction, uuid);
}

void PresetManager::sortBanks(UNDO::Transaction *transaction, const std::vector<Bank *> &banks)
{
  m_banks.sort(transaction, banks);
}

const std::set<int> &getUnavailableParametersBySoundType(SoundType t)
{
  static std::set<int> unavailableInSingleSound {
    C15::PID::FB_Mix_Comb_Src,   C15::PID::FB_Mix_FX_Src,        C15::PID::FB_Mix_Osc_Src,   C15::PID::Out_Mix_To_FX,
    C15::PID::Split_Split_Point, C15::PID::Voice_Grp_Fade_From,  C15::PID::Voice_Grp_Tune,   C15::PID::FB_Mix_Osc,
    C15::PID::FB_Mix_SVF_Src,    C15::PID::Voice_Grp_Fade_Range, C15::PID::Voice_Grp_Volume,
  };

  static std::set<int> unavailableInSplitSounds {
    C15::PID::FB_Mix_Comb_Src,     C15::PID::FB_Mix_FX_Src,        C15::PID::FB_Mix_Osc_Src, C15::PID::FB_Mix_SVF_Src,
    C15::PID::Voice_Grp_Fade_From, C15::PID::Voice_Grp_Fade_Range, C15::PID::FB_Mix_Osc,
  };

  static std::set<int> unavailableInLayerSounds { C15::PID::Split_Split_Point };

  switch(t)
  {
    case SoundType::Single:
      return std::ref(unavailableInSingleSound);

    case SoundType::Split:
      return std::ref(unavailableInSplitSounds);

    case SoundType::Layer:
      return std::ref(unavailableInLayerSounds);

    default:
      break;
  }
  return std::ref(unavailableInSingleSound);
}

void PresetManager::storeInitSound(UNDO::Transaction *transaction)
{
  auto currentState = std::make_unique<Preset>(this, *m_editBuffer);
  auto currentSoundType = m_editBuffer->getType();

  if(currentSoundType == SoundType::Single)
    currentState->copyVoiceGroup1IntoVoiceGroup2(transaction, {});

  static auto parameterGroupsToCloneInLayerSounds
      = std::set<GroupId> { GroupId { "Unison", VoiceGroup::I }, GroupId { "Mono", VoiceGroup::I } };

  if(currentSoundType == SoundType::Layer)
    currentState->copyVoiceGroup1IntoVoiceGroup2(transaction, parameterGroupsToCloneInLayerSounds);

  auto &takeFromRight = getUnavailableParametersBySoundType(currentSoundType);

  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    for(auto &currentGroup : currentState->getGroups(vg))
      for(auto &currentParameter : currentGroup.second->getParameters())
        if(takeFromRight.count(currentParameter.first.getNumber()))
          if(auto oldParameter = m_initSound->findParameterByID(currentParameter.first, false))
            currentParameter.second->copyFrom(transaction, oldParameter);

  m_initSound = std::move(currentState);
  m_editBuffer->undoableSetDefaultValues(transaction, m_initSound.get());
}

void PresetManager::loadInitSound(UNDO::Transaction *transaction)
{
  m_editBuffer->copyFrom(transaction, m_initSound.get());
}

void PresetManager::clear(UNDO::Transaction *transaction)
{
  while(getNumBanks())
    deleteBank(transaction, m_banks.first()->getUuid());

  selectBank(transaction, Uuid::none());
}

void PresetManager::setOrderNumber(UNDO::Transaction *transaction, const Uuid &bank, size_t targetPos)
{
  if(getNumBanks())
  {
    auto isSelected = getSelectedBankUuid() == bank;

    auto p = m_banks.release(transaction, bank);
    m_banks.adopt(transaction, targetPos, p);

    if(isSelected)
      selectBank(transaction, bank);
  }
}

void PresetManager::resetInitSound(UNDO::Transaction *transaction)
{
  auto eb = getEditBuffer();
  auto cleanPreset = std::make_unique<Preset>(this, *eb);

  cleanPreset->forEachParameter([&](PresetParameter *p) {
    auto src = eb->findParameterByID(p->getID());
    p->setValue(transaction, src->getValue().getFactoryDefaultValue());
  });

  auto swap = UNDO::createSwapData(std::move(cleanPreset));

  transaction->addSimpleCommand([swap, this](auto) {
    swap->swapWith(m_initSound);
    this->onChange();
  });

  m_editBuffer->undoableSetDefaultValues(transaction, m_initSound.get());
}

Glib::ustring PresetManager::createPresetNameBasedOn(const Glib::ustring &basedOn) const
{
  const Glib::ustring base = getBaseName(basedOn);

  if(base.empty())
    return "New preset";

  if(StringTools::hasEnding(base, "conv."))
  {
    return basedOn;
  }

  int highestPostfix = 0;
  m_banks.forEach([&](auto b) { highestPostfix = std::max(highestPostfix, b->getHighestIncrementForBaseName(base)); });

  if(highestPostfix == 0)
    return base;

  return base + " " + to_string(highestPostfix + 1);
}

void PresetManager::searchPresets(Writer &writer, const Glib::ustring &q, const Glib::ustring &mode,
                                  std::vector<SearchQuery::Fields> &&fieldsToSearch) const
{
  SearchQuery query(q, mode, std::move(fieldsToSearch));

  writer.writeTag("preset-manager", [&]() {
    writer.writeTag("banks", [&]() { m_banks.forEach([&](auto b) { b->searchPresets(writer, query); }); });
  });
}

Glib::ustring PresetManager::getBaseName(const Glib::ustring &basedOn) const
{
  auto regex = Glib::Regex::create("^(.*)\\s([0-9]+)$");
  Glib::MatchInfo matchInfo;

  if(regex->match(basedOn, matchInfo) && matchInfo.get_match_count() > 1)
    return matchInfo.fetch(1);

  return basedOn;
}

sigc::connection PresetManager::onBankSelection(sigc::slot<void, Uuid> cb)
{
  return m_sigBankSelection.connectAndInit(cb, m_banks.getSelectedUuid());
}

sigc::connection PresetManager::onNumBanksChanged(sigc::slot<void, size_t> cb)
{
  return m_sigNumBanksChanged.connectAndInit(cb, getNumBanks());
}

sigc::connection PresetManager::onRestoreHappened(sigc::slot<void> cb)
{
  return m_sigRestoreHappened.connect(cb);
}

std::pair<double, double> PresetManager::calcDefaultBankPositionForNewBank() const
{
  const Bank *rightMost = nullptr;

  m_banks.forEach([&](auto other) {
    if(!rightMost)
    {
      rightMost = other;
      return;
    }

    auto x = std::stod(other->getX());
    auto currentX = std::stod(rightMost->getX());

    if(x > currentX)
      rightMost = other;
  });

  if(rightMost)
    return std::make_pair(std::stod(rightMost->getX()) + 300, std::stod(rightMost->getY()));

  return std::make_pair(0.0, 0.0);
}

std::pair<double, double> PresetManager::calcDefaultBankPositionFor(const Bank *bank) const
{
  const Bank *rightMost = nullptr;

  m_banks.forEach([&](auto other) {
    if(other != bank)
    {
      if(!rightMost)
      {
        rightMost = other;
        return;
      }

      auto x = std::stod(other->getX());
      auto currentX = std::stod(rightMost->getX());

      if(x > currentX)
        rightMost = other;
    }
  });

  if(rightMost)
    return std::make_pair(std::stod(rightMost->getX()) + 300, std::stod(rightMost->getY()));

  return std::make_pair(0.0, 0.0);
}

size_t PresetManager::getBankPosition(const Uuid &uuid) const
{
  return m_banks.getIndexOf(uuid);
}

void PresetManager::sanitizeBankClusterRelations(UNDO::Transaction *transaction)
{
  resolveCyclicAttachments(transaction);
  ClusterEnforcement enforcer(this);
  enforcer.enforceClusterRuleOfOne(transaction);
}

void PresetManager::resolveCyclicAttachments(UNDO::Transaction *transaction)
{
  auto presetManager = Application::get().getPresetManager();
  auto allBanks = presetManager->getBanks();
  for(auto bank : allBanks)
  {
    bank->resolveCyclicAttachments(transaction);
  }
}

void PresetManager::ensureBankSelection(UNDO::Transaction *transaction)
{
  if(!getSelectedBank() && getNumBanks() > 0)
    selectBank(transaction, getBankAt(0)->getUuid());
}

void PresetManager::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("preset-manager", Attribute("changed", changed),
                  Attribute("file-version", VersionAttribute::getCurrentFileVersion()), [&]() {
                    if(changed)
                    {
                      m_editBuffer->writeDocument(writer, knownRevision);

                      bool anyBankChanged = didChangeSince(knownRevision);

                      if(!anyBankChanged)
                        forEachBank([&](Bank *bank) { anyBankChanged |= (bank->didChangeSince(knownRevision)); });

                      writer.writeTag("banks", Attribute("changed", anyBankChanged),
                                      Attribute("selected-bank", getSelectedBankUuid().raw()),
                                      Attribute("selected-midi-bank", getMidiSelectedBank().raw()), [&]() {
                                        if(anyBankChanged)
                                          forEachBank([&](auto bank) { bank->writeDocument(writer, knownRevision); });
                                      });
                    }
                  });
}

void PresetManager::stress(int numTransactions)
{
  Glib::MainContext::get_default()->signal_timeout().connect_once(
      [=]() {
        {
          auto transactionScope = getUndoScope().startTransaction("Stressing Undo System");
          m_editBuffer->undoableSelectParameter(transactionScope->getTransaction(),
                                                { C15::PID::FB_Mix_FX_Src, VoiceGroup::I }, SignalOrigin::EXPLICIT);

          if(auto p = m_editBuffer->getSelected(VoiceGroup::I))
          {
            p->stepCPFromHwui(transactionScope->getTransaction(), g_random_boolean() ? -1 : 1, ButtonModifiers());
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
      },
      0);
}

void PresetManager::stressParam(UNDO::Transaction *trans, Parameter *param)
{
  if(m_editBuffer->getSelected(VoiceGroup::I) != param)
  {
    m_editBuffer->undoableSelectParameter(trans, param, SignalOrigin::EXPLICIT);
  }
  param->stepCPFromHwui(trans, g_random_boolean() ? -1 : 1, ButtonModifiers {});
}

void PresetManager::stressAllParams(int numParamChangedForEachParameter)
{
  Glib::MainContext::get_default()->signal_timeout().connect_once(
      [=]() {
        auto scope = getUndoScope().startTransaction("Stress All Parameters");
        auto trans = scope->getTransaction();

        for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
          for(auto &group : m_editBuffer->getParameterGroups(vg))
            for(auto &param : group->getParameters())
              for(auto i = 0; i < numParamChangedForEachParameter; i++)
                stressParam(trans, param);
      },
      0);
}

void PresetManager::stressBlocking(int numTransactions)
{
  int parameterId = g_random_int_range(0, 200);
  {
    auto transactionScope = getUndoScope().startTransaction("Stressing Undo System");
    m_editBuffer->undoableSelectParameter(transactionScope->getTransaction(), { parameterId, VoiceGroup::I },
                                          SignalOrigin::EXPLICIT);

    if(auto p = m_editBuffer->getSelected(VoiceGroup::I))
    {
      p->stepCPFromHwui(transactionScope->getTransaction(), g_random_boolean() ? -1 : 1, ButtonModifiers());
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
    stressBlocking(numTransactions - 1);
  }
}

void PresetManager::stressLoad(int numTransactions)
{
  Glib::MainContext::get_default()->signal_timeout().connect_once(
      [=]() {
        int numSteps = numTransactions;
        auto transactionScope = getUndoScope().startTransaction("Stressing by Preset loading");
        auto transaction = transactionScope->getTransaction();

        while(numSteps > 0)
        {
          forEachBank([&](auto b) {
            b->forEachPreset([&](auto p) {
              m_editBuffer->undoableLoad(transaction, p, true);
              numSteps--;
            });
          });
        }
      },
      0);
}

void PresetManager::incAllParamsFine()
{
  Glib::MainContext::get_default()->signal_timeout().connect_once(
      [=]() {
        auto scope = getUndoScope().startTransaction("Inc All Parameters Fine");
        auto trans = scope->getTransaction();

        for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
          for(auto &group : m_editBuffer->getParameterGroups(vg))
            for(auto &param : group->getParameters())
              param->stepCPFromHwui(trans, 1, ButtonModifiers { ButtonModifier::FINE });
      },
      0);
}

const Preset *PresetManager::getSelectedPreset() const
{
  if(auto bank = getSelectedBank())
  {
    return bank->getSelectedPreset();
  }
  return nullptr;
}

Preset *PresetManager::getSelectedPreset()
{
  if(auto bank = getSelectedBank())
  {
    return bank->getSelectedPreset();
  }
  return nullptr;
}

void PresetManager::undoableLoadSelectedPresetAccordingToLoadType(UNDO::Transaction *transaction) const
{
  nltools_assertAlways(transaction != nullptr);

  auto eb = getEditBuffer();
  auto hwui = Application::get().getHWUI();
  auto currentVoiceGroup = hwui->getCurrentVoiceGroup();

  if(auto bank = getSelectedBank())
  {
    if(auto selPreset = bank->getSelectedPreset())
    {
      auto directLoadSetting = Application::get().getSettings()->getSetting<DirectLoadSetting>();
      auto loadToPartActive = Application::get().getHWUI()->isInLoadToPart();

      switch(selPreset->getType())
      {
        case SoundType::Single:
          eb->undoableLoadSelectedPreset(transaction, currentVoiceGroup);

          break;
        case SoundType::Layer:
        case SoundType::Split:
          if(loadToPartActive)
          {
            auto load = hwui->getPresetPartSelection(currentVoiceGroup);
            eb->undoableLoadToPart(transaction, load->m_preset, load->m_voiceGroup, currentVoiceGroup);
          }
          else
          {
            eb->undoableLoadSelectedPreset(transaction, currentVoiceGroup);
          }
          break;
        default:
          break;
      }
    }
  }
}

void PresetManager::onPresetStored()
{
  m_presetStoreHappened.deferedSend();
}

sigc::connection PresetManager::onPresetStoreHappened(sigc::slot<void> cb)
{
  return m_presetStoreHappened.connect(cb);
}

Uuid PresetManager::getMidiSelectedBank() const
{
  return m_midiSelectedBank;
}

void PresetManager::selectMidiBank(UNDO::Transaction *trans, const Uuid &uuid)
{
  auto swapData = UNDO::createSwapData(uuid);

  trans->addSimpleCommand([=, newUuid = uuid](auto) {
    const auto oldMidiBankUuid = m_midiSelectedBank;

    swapData->swapWith(m_midiSelectedBank);
    m_sigMidiBankSelection.send(m_midiSelectedBank);

    if(auto oldMidiBank = findBank(oldMidiBankUuid))
      oldMidiBank->invalidate();

    if(auto newMidiBank = findBank(newUuid))
      newMidiBank->invalidate();
  });
}

sigc::connection PresetManager::onMidiBankSelectionHappened(sigc::slot<void, Uuid> cb)
{
  return m_sigMidiBankSelection.connectAndInit(cb, getMidiSelectedBank());
}

Bank *PresetManager::findMidiSelectedBank() const
{
  return m_banks.find(getMidiSelectedBank());
}

sigc::connection PresetManager::onLoadHappened(sigc::slot<void> cb)
{
  return m_sigLoadHappened.connect(cb);
}
