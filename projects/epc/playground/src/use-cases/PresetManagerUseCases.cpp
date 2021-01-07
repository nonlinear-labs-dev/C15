#include <Application.h>
#include <parameter_declarations.h>
#include <device-settings/SyncSplitSettingUseCases.h>
#include <presets/ClusterEnforcement.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <xml/MemoryInStream.h>
#include <device-settings/DebugLevel.h>
#include <presets/SendEditBufferScopeGuard.h>
#include <device-settings/DirectLoadSetting.h>
#include <device-settings/BooleanSetting.h>
#include "PresetManagerUseCases.h"
#include "presets/PresetManager.h"
#include "presets/Preset.h"
#include "presets/Bank.h"
#include "presets/EditBuffer.h"
#include "libundo/undo/Scope.h"
#include "http/UndoScope.h"
#include "libundo/undo/Transaction.h"
#include "device-settings/Settings.h"
#include "device-settings/SplitPointSyncParameters.h"
#include "parameters/SplitPointParameter.h"
#include <serialization/PresetManagerSerializer.h>
#include <serialization/PresetSerializer.h>
#include <xml/VersionAttribute.h>

PresetManagerUseCases::PresetManagerUseCases(PresetManager* pm)
    : m_presetManager { pm }
{
}

void PresetManagerUseCases::overwritePreset(const Uuid& uuid)
{
  if(auto preset = m_presetManager->findPreset(uuid))
  {
    overwritePreset(preset);
  }
}

void PresetManagerUseCases::overwritePreset(Preset* preset)
{
  auto scope = Application::get().getUndoScope()->startTransaction("Overwrite preset '%0'", preset->getName());
  auto transaction = scope->getTransaction();

  if(auto bank = dynamic_cast<Bank*>(preset->getParent()))
  {
    preset->copyFrom(transaction, m_presetManager->getEditBuffer());
    m_presetManager->getEditBuffer()->undoableLoad(transaction, preset, false);
    bank->selectPreset(transaction, preset->getUuid());
    m_presetManager->selectBank(transaction, bank->getUuid());

    onStore(transaction, preset);

    assert(m_presetManager->getSelectedBank() == bank);
    assert(bank->getSelectedPreset() == preset);
    assert(m_presetManager->getEditBuffer()->getUUIDOfLastLoadedPreset() == preset->getUuid());
  }
}

void PresetManagerUseCases::overwritePresetWithPreset(Preset* target, Preset* source)
{
  auto scope = Application::get().getUndoScope()->startTransaction("Overwrite preset '%0' with '%1'", target->getName(),
                                                                   source->getName());
  if(auto targetBank = dynamic_cast<Bank*>(target->getParent()))
  {
    auto transaction = scope->getTransaction();
    target->copyFrom(transaction, source);
    targetBank->selectPreset(transaction, target->getUuid());
    m_presetManager->selectBank(transaction, targetBank->getUuid());

    onStore(transaction, target);

    assert(m_presetManager->getSelectedBank() == targetBank);
    assert(targetBank->getSelectedPreset() == target);
  }
}

void PresetManagerUseCases::insertPresetWithUUID(Bank* bank, size_t pos, const std::string& uuid)
{
  Uuid id;

  if(uuid.empty())
  {
    id.generate();
  }
  else
  {
    id = uuid;
  }

  auto pm = Application::get().getPresetManager();
  auto scope = Application::get().getUndoScope()->startTransaction("Insert preset at position %0", pos + 1);
  auto transaction = scope->getTransaction();
  auto ebIsModified = m_presetManager->getEditBuffer()->isModified();

  auto preset = bank->insertAndLoadPreset(transaction, pos, std::make_unique<Preset>(bank, *pm->getEditBuffer(), id));

  m_presetManager->selectBank(transaction, bank->getUuid());
  bank->selectPreset(transaction, preset->getUuid());

  if(ebIsModified)
    preset->guessName(transaction);

  onStore(transaction, preset);

  assert(m_presetManager->getSelectedBank() == bank);
  assert(bank->getSelectedPreset() == preset);
  assert(m_presetManager->getEditBuffer()->getUUIDOfLastLoadedPreset() == preset->getUuid());
}

void PresetManagerUseCases::insertPreset(Bank* bank, size_t pos)
{
  insertPresetWithUUID(bank, pos, "");
}

void PresetManagerUseCases::appendPreset(Bank* bank)
{
  insertPresetWithUUID(bank, bank->getNumPresets(), "");
}

void PresetManagerUseCases::appendPresetWithUUID(Bank* bank, const std::string& uuid)
{
  insertPresetWithUUID(bank, bank->getNumPresets(), uuid);
}

void PresetManagerUseCases::createBankAndStoreEditBuffer()
{
  auto& scope = m_presetManager->getUndoScope();
  auto transactionScope = scope.startTransaction("Create Bank and Store Preset");
  auto transaction = transactionScope->getTransaction();
  auto bank = m_presetManager->addBank(transaction);
  auto preset
      = bank->appendAndLoadPreset(transaction, std::make_unique<Preset>(bank, *m_presetManager->getEditBuffer()));
  bank->selectPreset(transaction, preset->getUuid());
  m_presetManager->selectBank(transaction, bank->getUuid());

  onStore(transaction, preset);
}

void PresetManagerUseCases::onStore(UNDO::Transaction* transaction, Preset* preset)
{
  if(preset->getType() == SoundType::Split)
  {
    updateSyncSettingOnPresetStore(transaction);
  }

  m_presetManager->onPresetStored();
}

void PresetManagerUseCases::updateSyncSettingOnPresetStore(UNDO::Transaction* transaction) const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  if(auto s = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I }))
  {
    if(s->hasOverlap())
    {
      SyncSplitSettingUseCases::get().disableSyncSetting(transaction);
    }
    else
    {
      SyncSplitSettingUseCases::get().enableSyncSetting(transaction);
    }
  }
}

void PresetManagerUseCases::newBank(const Glib::ustring& x, const Glib::ustring& y, const Glib::ustring& name)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("New Bank");
  auto transaction = scope->getTransaction();
  auto bank = m_presetManager->addBank(transaction);
  bank->setX(transaction, x);
  bank->setY(transaction, y);
  bank->setName(scope->getTransaction(), name);
  m_presetManager->selectBank(scope->getTransaction(), bank->getUuid());
}

void PresetManagerUseCases::newBank(const Glib::ustring& x, const Glib::ustring& y)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("New Bank");
  auto transaction = scope->getTransaction();
  auto bank = m_presetManager->addBank(transaction);
  bank->setX(transaction, x);
  bank->setY(transaction, y);
  auto preset
      = bank->appendAndLoadPreset(transaction, std::make_unique<Preset>(bank, m_presetManager->getEditBuffer(), false));
  bank->selectPreset(transaction, preset->getUuid());
  m_presetManager->selectBank(transaction, bank->getUuid());
}

void PresetManagerUseCases::selectBank(Bank* bank)
{
  if(bank)
  {
    selectBank(bank->getUuid());
  }
}

void PresetManagerUseCases::selectBank(const Uuid& uuid)
{
  if(auto bank = m_presetManager->findBank(uuid))
  {
    auto& undoScope = m_presetManager->getUndoScope();
    auto transactionScope = undoScope.startTransaction("Select Bank '%0'", bank->getName(true));
    auto transaction = transactionScope->getTransaction();
    m_presetManager->selectBank(transaction, uuid);
  }
}

void PresetManagerUseCases::selectBank(int idx)
{
  if(idx < m_presetManager->getNumBanks())
  {
    auto bank = m_presetManager->getBankAt(idx);
    auto transactionScope = m_presetManager->getUndoScope().startTransaction("Select Bank '%0'", bank->getName(true));
    m_presetManager->selectBank(transactionScope->getTransaction(), bank->getUuid());
  }
}

void PresetManagerUseCases::deleteBank(const Uuid& uuid)
{
  if(auto bank = m_presetManager->findBank(uuid))
  {
    auto scope = m_presetManager->getUndoScope().startTransaction("Delete Bank '%0'", bank->getName(true));
    auto transaction = scope->getTransaction();

    if(m_presetManager->getSelectedBankUuid() == uuid)
      if(!m_presetManager->selectPreviousBank(transaction))
        m_presetManager->selectNextBank(transaction);

    m_presetManager->deleteBank(transaction, uuid);
  }
}

void PresetManagerUseCases::moveBankCluster(std::vector<std::string> uuids)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Moved Banks");
  auto transaction = scope->getTransaction();

  nltools_assertAlways(uuids.size() % 3 == 0);

  for(auto i = uuids.begin(); i != uuids.end();)
  {
    if(auto selBank = m_presetManager->findBank(*(i++)))
    {
      auto x = *(i++);
      auto y = *(i++);
      selBank->setX(transaction, x);
      selBank->setY(transaction, y);
    }
    else
    {
      std::advance(i, 2);
    }
  }
}

void PresetManagerUseCases::selectMidiBank(Bank* b)
{
  if(b)
  {
    auto& scope = m_presetManager->getUndoScope();
    auto transactionScope = scope.startTransaction("Select Bank '%0' for Midi Program-Changes", b->getName(true));
    auto transaction = transactionScope->getTransaction();
    m_presetManager->selectMidiBank(transaction, b->getUuid());
  }
  else
  {
    auto& scope = m_presetManager->getUndoScope();
    auto transactionScope = scope.startTransaction("Remove Bank for Midi Program-Changes");
    auto transaction = transactionScope->getTransaction();
    m_presetManager->selectMidiBank(transaction, "");
  }
}

void PresetManagerUseCases::deleteBank(Bank* b)
{
  if(b)
  {
    auto& scope = m_presetManager->getUndoScope();
    auto transScope = scope.startTransaction("Delete Bank '%0'", b->getName(true));
    auto transaction = transScope->getTransaction();

    //Update Bank Selection
    if(m_presetManager->getSelectedBankUuid() == b->getUuid())
      if(!m_presetManager->selectPreviousBank(transaction))
        m_presetManager->selectNextBank(transaction);

    //Clear Midi bank
    if(m_presetManager->getMidiSelectedBank() == b->getUuid())
      m_presetManager->selectMidiBank(transaction, Uuid::none());

    m_presetManager->deleteBank(transaction, b->getUuid());
    m_presetManager->getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
  }
}

void PresetManagerUseCases::deletePresets(const std::vector<std::string>& uuids, bool deleteBanks)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Delete Presets");
  auto transaction = scope->getTransaction();
  for(auto& p : uuids)
  {
    if(auto bank = m_presetManager->findBankWithPreset(p))
    {
      if(auto preset = bank->findPreset(p))
      {
        bank->deletePreset(transaction, preset->getUuid());
        if(deleteBanks && bank->getNumPresets() == 0)
        {
          m_presetManager->deleteBank(transaction, bank->getUuid());
        }
      }
    }
  }

  m_presetManager->getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
}

void PresetManagerUseCases::selectPreset(const Uuid& uuid)
{
  if(auto preset = m_presetManager->findPreset(uuid))
  {
    selectPreset(preset);
  }
}

void PresetManagerUseCases::selectPreset(const Preset* preset)
{
  if(preset)
  {
    const auto& presetUuid = preset->getUuid();
    if(auto bank = m_presetManager->findBankWithPreset(presetUuid))
    {
      auto scope = Application::get().getUndoScope()->startTransaction("Select Preset");
      bank->selectPreset(scope->getTransaction(), presetUuid);
    }
  }
}
void PresetManagerUseCases::moveLeft(Bank* bank)
{
  if(bank)
  {
    auto pos = m_presetManager->getBankPosition(bank->getUuid());
    if(pos > 0)
    {
      auto scope = bank->getUndoScope().startTransaction("Move bank '%0' left", bank->getName(true));
      m_presetManager->setOrderNumber(scope->getTransaction(), bank->getUuid(), pos - 1);
    }
  }
}

void PresetManagerUseCases::moveRight(Bank* bank)
{
  if(bank)
  {
    auto pos = m_presetManager->getBankPosition(bank->getUuid()) + 1;
    if(pos < m_presetManager->getNumBanks())
    {
      auto scope = bank->getUndoScope().startTransaction("Move bank '%0' right", bank->getName(true));
      m_presetManager->setOrderNumber(scope->getTransaction(), bank->getUuid(), pos);
    }
  }
}

void PresetManagerUseCases::movePresetAbove(const Uuid& presetToMoveUuid, const Uuid& presetAnchorUuid)
{
  auto srcBank = m_presetManager->findBankWithPreset(presetToMoveUuid);
  auto tgtBank = m_presetManager->findBankWithPreset(presetAnchorUuid);

  if(srcBank && tgtBank)
  {
    if(auto toMove = srcBank->findPreset(presetToMoveUuid))
    {
      auto scope = m_presetManager->getUndoScope().startTransaction("Move preset");
      auto transaction = scope->getTransaction();
      auto anchor = tgtBank->findPresetNear(presetAnchorUuid, 0);
      srcBank->movePresetBetweenBanks(transaction, toMove, tgtBank, anchor);
      tgtBank->selectPreset(transaction, presetToMoveUuid);
      m_presetManager->selectBank(transaction, tgtBank->getUuid());
    }
  }
}

void PresetManagerUseCases::movePresetBelow(const Uuid& presetToMoveUuid, const Uuid& presetAnchorUuid)
{
  auto srcBank = m_presetManager->findBankWithPreset(presetToMoveUuid);
  auto tgtBank = m_presetManager->findBankWithPreset(presetAnchorUuid);

  if(srcBank && tgtBank)
  {
    if(auto toMove = srcBank->findPreset(presetToMoveUuid))
    {
      auto scope = m_presetManager->getUndoScope().startTransaction("Move preset");
      auto transaction = scope->getTransaction();
      auto anchor = tgtBank->findPresetNear(presetAnchorUuid, 1);
      srcBank->movePresetBetweenBanks(transaction, toMove, tgtBank, anchor);
      tgtBank->selectPreset(transaction, presetToMoveUuid);
      m_presetManager->selectBank(transaction, tgtBank->getUuid());
    }
  }
}

void PresetManagerUseCases::movePresetTo(const Uuid& overwriteWith, const Uuid& presetToOverwrite)
{
  auto tgtBank = m_presetManager->findBankWithPreset(presetToOverwrite);
  auto srcBank = m_presetManager->findBankWithPreset(overwriteWith);

  if(srcBank && tgtBank)
  {
    auto srcPreset = srcBank->findPreset(overwriteWith);
    auto tgtPreset = tgtBank->findPreset(presetToOverwrite);

    if(srcPreset != tgtPreset)
    {
      auto scope = m_presetManager->getUndoScope().startTransaction("Overwrite preset");
      auto transaction = scope->getTransaction();

      auto anchor = tgtBank->findPresetNear(presetToOverwrite, 0);
      tgtBank->movePresetBetweenBanks(transaction, srcPreset, tgtBank, anchor);
      srcBank->deletePreset(transaction, presetToOverwrite);
      tgtBank->selectPreset(transaction, srcPreset->getUuid());
      m_presetManager->selectBank(transaction, tgtBank->getUuid());
    }
  }
}

void PresetManagerUseCases::setOrderNumber(Bank* b, int newOrderNumber)
{
  if(b)
  {
    auto scope = m_presetManager->getUndoScope().startTransaction("Changed Order Number of Bank: %0", b->getName(true));
    m_presetManager->setOrderNumber(scope->getTransaction(), b->getUuid(), newOrderNumber);
  }
}

void PresetManagerUseCases::stepBankSelection(int inc, bool shift)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Select Bank");

  if(shift && m_presetManager->getNumBanks() > 0)
  {
    if(inc < 0)
      m_presetManager->selectBank(scope->getTransaction(), m_presetManager->getBanks().front()->getUuid());
    else
      m_presetManager->selectBank(scope->getTransaction(), m_presetManager->getBanks().back()->getUuid());
  }
  else
  {
    while(inc < 0)
    {
      m_presetManager->selectPreviousBank(scope->getTransaction());
      inc++;
    }

    while(inc > 0)
    {
      m_presetManager->selectNextBank(scope->getTransaction());
      inc--;
    }
  }
}

void PresetManagerUseCases::sortBankNumbers()
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Sort Bank Numbers");
  auto transaction = scope->getTransaction();

  ClusterEnforcement ce(m_presetManager);
  auto newBanks = ce.sortBanks();
  m_presetManager->sortBanks(transaction, newBanks);
}

void PresetManagerUseCases::dropPresets(const std::string& anchorUuid, PresetManagerUseCases::DropActions action,
                                        const Glib::ustring& csv)
{
  auto actionToOffset = [](DropActions action) {
    switch(action)
    {
      case DropActions::Above:
        return 0;
      case DropActions::Below:
        return 1;
      case DropActions::Onto:
        return 1;
    }

    nltools_assertNotReached();
  };

  int offset = actionToOffset(action);

  std::vector<std::string> strs;
  boost::split(strs, csv, boost::is_any_of(","));

  if(auto anchor = m_presetManager->findPreset(anchorUuid))
  {
    auto scope = m_presetManager->getUndoScope().startTransaction("Drop Presets");
    auto transaction = scope->getTransaction();

    auto bank = static_cast<Bank*>(anchor->getParent());
    auto anchorPos = static_cast<int>(bank->getPresetPosition(anchorUuid));
    auto pos = std::max(anchorPos + offset, 0);

    for(const auto& presetUUID : strs)
    {
      if(auto src = m_presetManager->findPreset(presetUUID))
      {
        if(src->getParent() == anchor->getParent())
        {
          bank->movePreset(transaction, src, anchor);
        }
        else
        {
          bank->insertPreset(transaction, size_t(pos), std::make_unique<Preset>(bank, *src, true));
          pos++;
        }
      }
    }

    if(action == DropActions::Onto)
    {
      if(auto droppedOntoPreset = m_presetManager->findPreset(anchorUuid))
      {
        static_cast<Bank*>(droppedOntoPreset->getParent())->deletePreset(transaction, anchorUuid);
      }
    }
  }
}

void PresetManagerUseCases::copyPresetAbove(const Uuid& presetToCopy, const Uuid& presetAnchorUuid)
{
  auto srcBank = m_presetManager->findBankWithPreset(presetToCopy);
  auto tgtBank = m_presetManager->findBankWithPreset(presetAnchorUuid);

  if(srcBank && tgtBank)
  {
    auto srcPreset = srcBank->findPreset(presetToCopy);
    auto anchorPos = tgtBank->getPresetPosition(presetAnchorUuid);

    nltools_assertAlways(srcPreset);

    auto scope = m_presetManager->getUndoScope().startTransaction("Copy preset '%0'", srcPreset->getName());
    auto transaction = scope->getTransaction();
    auto newPreset = std::make_unique<Preset>(tgtBank, *srcPreset, true);
    auto tgtPreset = tgtBank->insertPreset(transaction, anchorPos, std::move(newPreset));
    tgtBank->selectPreset(transaction, tgtPreset->getUuid());
    m_presetManager->selectBank(transaction, tgtBank->getUuid());
  }
}

void PresetManagerUseCases::copyPresetBelow(const Uuid& presetToCopy, const Uuid& presetAnchorUuid)
{
  auto srcBank = m_presetManager->findBankWithPreset(presetToCopy);
  auto tgtBank = m_presetManager->findBankWithPreset(presetAnchorUuid);

  if(srcBank && tgtBank)
  {
    auto srcPreset = srcBank->findPreset(presetToCopy);
    auto anchorPos = tgtBank->getPresetPosition(presetAnchorUuid) + 1;

    nltools_assertAlways(srcPreset);

    auto scope = m_presetManager->getUndoScope().startTransaction("Copy preset '%0'", srcPreset->getName());
    auto transaction = scope->getTransaction();
    auto newPreset = std::make_unique<Preset>(tgtBank, *srcPreset, true);
    auto tgtPreset = tgtBank->insertPreset(transaction, anchorPos, std::move(newPreset));
    tgtBank->selectPreset(transaction, tgtPreset->getUuid());
    m_presetManager->selectBank(transaction, tgtBank->getUuid());
  }
}

std::string guessNameBasedOnEditBuffer(EditBuffer*);

void PresetManagerUseCases::insertEditBufferAbove(const Uuid& anchor, const Uuid& ebUuid)
{

  if(auto tgtBank = m_presetManager->findBankWithPreset(anchor))
  {
    auto name = guessNameBasedOnEditBuffer(m_presetManager->getEditBuffer());
    auto anchorPos = tgtBank->getPresetPosition(anchor);
    auto& undoScope = m_presetManager->getUndoScope();
    auto transactionScope = undoScope.startTransaction("Save new Preset in Bank '%0'", tgtBank->getName(true));
    auto transaction = transactionScope->getTransaction();
    auto newPreset = std::make_unique<Preset>(tgtBank, *m_presetManager->getEditBuffer());
    auto tgtPreset = tgtBank->insertAndLoadPreset(transaction, anchorPos, std::move(newPreset));
    tgtPreset->setName(transaction, name);

    if(!ebUuid.empty())
      tgtPreset->setUuid(transaction, ebUuid);

    tgtBank->selectPreset(transaction, tgtPreset->getUuid());
    m_presetManager->selectBank(transaction, tgtBank->getUuid());
  }
}

void PresetManagerUseCases::insertEditBufferBelow(const Uuid& anchor, const Uuid& ebUuid)
{
  if(auto tgtBank = m_presetManager->findBankWithPreset(anchor))
  {
    auto name = guessNameBasedOnEditBuffer(m_presetManager->getEditBuffer());
    auto anchorPos = tgtBank->getPresetPosition(anchor) + 1;
    auto& undoScope = m_presetManager->getUndoScope();
    auto transactionScope = undoScope.startTransaction("Save new Preset in Bank '%0'", tgtBank->getName(true));
    auto transaction = transactionScope->getTransaction();
    auto newPreset = std::make_unique<Preset>(tgtBank, *m_presetManager->getEditBuffer());
    auto tgtPreset = tgtBank->insertAndLoadPreset(transaction, anchorPos, std::move(newPreset));
    tgtPreset->setName(transaction, name);

    if(!ebUuid.empty())
      tgtPreset->setUuid(transaction, ebUuid);

    tgtBank->selectPreset(transaction, tgtPreset->getUuid());
    m_presetManager->selectBank(transaction, tgtBank->getUuid());
  }
}

void PresetManagerUseCases::createBankFromPreset(const Uuid& uuid, const std::string& x, const std::string& y)
{
  if(auto bank = m_presetManager->findBankWithPreset(uuid))
  {
    if(auto p = bank->findPreset(uuid))
    {
      auto scope = m_presetManager->getUndoScope().startTransaction("Create new bank");
      auto transaction = scope->getTransaction();
      auto newBank = m_presetManager->addBank(transaction);
      newBank->setX(transaction, x);
      newBank->setY(transaction, y);
      auto newPreset = newBank->appendPreset(transaction, std::make_unique<Preset>(newBank, *p, true));
      newBank->setName(transaction, "New Bank");
      newBank->selectPreset(transaction, newPreset->getUuid());
      m_presetManager->selectBank(transaction, newBank->getUuid());
    }
  }
}

void PresetManagerUseCases::createBankFromPresets(const std::string& csv, const std::string& x, const std::string& y)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Create new bank");
  auto transaction = scope->getTransaction();

  auto newBank = m_presetManager->addBank(transaction);
  newBank->setX(transaction, x);
  newBank->setY(transaction, y);
  newBank->setName(transaction, "New Bank");

  std::vector<std::string> strs;
  boost::split(strs, csv, boost::is_any_of(","));

  for(const auto& uuid : strs)
    if(auto src = m_presetManager->findPreset(uuid))
      newBank->appendPreset(transaction, std::make_unique<Preset>(newBank, *src, true));

  if(newBank->getNumPresets() > 0)
    newBank->selectPreset(transaction, newBank->getPresetAt(0)->getUuid());

  m_presetManager->selectBank(transaction, newBank->getUuid());
}

PresetManagerUseCases::ImportExitCode PresetManagerUseCases::importBackupFile(FileInStream& in)
{
  if(!in.eof())
  {
    auto scope = m_presetManager->getUndoScope().startTransaction("Import Presetmanager Backup");
    m_presetManager->clear(scope->getTransaction());
    PresetManagerSerializer serializer(m_presetManager);

    XmlReader reader(in, scope->getTransaction());
    reader.onFileVersionRead([&](int version) {
      if(version > VersionAttribute::getCurrentFileVersion())
      {
        scope->getTransaction()->rollBack();
        return Reader::FileVersionCheckResult::Unsupported;
      }
      return Reader::FileVersionCheckResult::OK;
    });

    if(auto lock = m_presetManager->lockLoading())
    {
      if(reader.read<PresetManagerSerializer>(m_presetManager))
      {
        m_presetManager->getEditBuffer()->sendToAudioEngine();
        return ImportExitCode::OK;
      }
    }
  }
  return ImportExitCode::Unsupported;
}

bool PresetManagerUseCases::importBackupFile(SoupBuffer* buffer)
{
  if(auto lock = m_presetManager->getLoadingLock())
  {
    auto scope = m_presetManager->getUndoScope().startTransaction("Import all Banks");
    auto transaction = scope->getTransaction();
    m_presetManager->clear(transaction);

    MemoryInStream stream(buffer, true);
    XmlReader reader(stream, transaction);

    if(!reader.read<PresetManagerSerializer>(m_presetManager))
    {
      transaction->rollBack();
      return false;
    }

    m_presetManager->getEditBuffer()->sendToAudioEngine();
    return true;
  }
  return false;
}

bool PresetManagerUseCases::loadPresetFromCompareXML(const Glib::ustring& xml)
{
  auto editBuffer = m_presetManager->getEditBuffer();

  Preset p(editBuffer);

  {
    auto scope = UNDO::Scope::startTrashTransaction();
    auto transaction = scope->getTransaction();

    MemoryInStream stream(xml, false);
    XmlReader reader(stream, transaction);

    if(!reader.read<PresetSerializer>(&p))
    {
      DebugLevel::warning("Could not read Preset xml!");
      return false;
    }
  }

  auto loadscope = m_presetManager->getUndoScope().startTransaction("Load Compare Buffer");
  auto loadtransaction = loadscope->getTransaction();

  SendEditBufferScopeGuard scopeGuard(loadtransaction, true);

  auto autoLoadSetting = Application::get().getSettings()->getSetting<DirectLoadSetting>();
  auto scopedLock = autoLoadSetting->scopedOverlay(BooleanSettings::BOOLEAN_SETTING_FALSE);
  editBuffer->copyFrom(loadtransaction, &p);
  editBuffer->undoableSetLoadedPresetInfo(loadtransaction, &p);
  return true;
}

void PresetManagerUseCases::insertBankInCluster(Bank* bankToInsert, Bank* bankAtInsert,
                                                const Glib::ustring& directionSeenFromBankInCluster)
{
  auto name = bankToInsert->getName(true);
  auto scope = m_presetManager->getUndoScope().startTransaction("Insert Bank %0 into Cluster", name);
  auto transaction = scope->getTransaction();

  if(auto slaveBottom = bankToInsert->getSlaveBottom())
    slaveBottom->attachBank(transaction, Uuid::none(), Bank::AttachmentDirection::none);

  if(auto slaveRight = bankToInsert->getSlaveRight())
    slaveRight->attachBank(transaction, Uuid::none(), Bank::AttachmentDirection::none);

  if(directionSeenFromBankInCluster == "North")
  {
    if(auto topMaster = bankAtInsert->getMasterTop())
    {
      bankToInsert->attachBank(transaction, topMaster->getUuid(), Bank::AttachmentDirection::top);
    }
    bankAtInsert->attachBank(transaction, bankToInsert->getUuid(), Bank::AttachmentDirection::top);
  }
  else if(directionSeenFromBankInCluster == "South")
  {
    if(auto bottomSlave = bankAtInsert->getSlaveBottom())
    {
      bottomSlave->attachBank(transaction, bankToInsert->getUuid(), Bank::AttachmentDirection::top);
    }
    bankToInsert->attachBank(transaction, bankAtInsert->getUuid(), Bank::AttachmentDirection::top);
  }
  else if(directionSeenFromBankInCluster == "East")
  {
    if(auto rightSlave = bankAtInsert->getSlaveRight())
    {
      rightSlave->attachBank(transaction, bankToInsert->getUuid(), Bank::AttachmentDirection::left);
    }
    bankToInsert->attachBank(transaction, bankAtInsert->getUuid(), Bank::AttachmentDirection::left);
  }
  else if(directionSeenFromBankInCluster == "West")
  {
    if(auto leftMaster = bankAtInsert->getMasterLeft())
    {
      bankToInsert->attachBank(transaction, leftMaster->getUuid(), Bank::AttachmentDirection::left);
    }
    bankAtInsert->attachBank(transaction, bankToInsert->getUuid(), Bank::AttachmentDirection::left);
  }
}

void PresetManagerUseCases::moveAllBanks(float x, float y)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Move all Banks");
  auto transaction = scope->getTransaction();

  for(auto bank : m_presetManager->getBanks())
  {
    bank->setX(transaction, to_string(std::stoi(bank->getX()) + x));
    bank->setY(transaction, to_string(std::stoi(bank->getY()) + y));
  }
}

void PresetManagerUseCases::pastePresetOnBank(Bank* bank, const Preset* preset, Clipboard* pClipboard)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Paste Preset");
  auto transaction = scope->getTransaction();
  bank->appendPreset(transaction, std::make_unique<Preset>(bank, *preset, true));
  pClipboard->doCut(transaction);
}

void PresetManagerUseCases::pastePresetOnPreset(Preset* target, Preset* source, Clipboard* clipboard)
{
  if(auto targetBank = dynamic_cast<Bank*>(target->getParent()))
  {
    auto scope = m_presetManager->getUndoScope().startTransaction("Paste Preset");
    auto transaction = scope->getTransaction();
    auto insertPos = targetBank->getPresetPosition(target->getUuid()) + 1;
    auto newPreset = std::make_unique<Preset>(targetBank, *source, true);
    auto newPresetPtr = targetBank->insertPreset(transaction, insertPos, std::move(newPreset));
    targetBank->selectPreset(transaction, newPresetPtr->getUuid());
    clipboard->doCut(transaction);
  }
}

void PresetManagerUseCases::pasteBankOnBackground(const Glib::ustring& name, const Glib::ustring& x,
                                                  const Glib::ustring& y, const Bank* source, Clipboard* pClipboard)
{
  auto scope = m_presetManager->getUndoScope().startTransaction(name);
  auto transaction = scope->getTransaction();
  auto newBank = m_presetManager->addBank(transaction, std::make_unique<Bank>(m_presetManager, *source, true));
  newBank->setX(transaction, x);
  newBank->setY(transaction, y);
  m_presetManager->selectBank(transaction, newBank->getUuid());
  pClipboard->doCut(transaction);
}

void PresetManagerUseCases::pastePresetOnBackground(const Glib::ustring& x, const Glib::ustring& y, Preset* source,
                                                    Clipboard* clipboard)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Paste Preset");
  auto transaction = scope->getTransaction();
  auto newBank = m_presetManager->addBank(transaction);
  newBank->setX(transaction, x);
  newBank->setY(transaction, y);
  newBank->prependPreset(transaction, std::make_unique<Preset>(newBank, *source, true));
  newBank->ensurePresetSelection(transaction);
  m_presetManager->selectBank(transaction, newBank->getUuid());
  clipboard->doCut(transaction);
}

std::string guessNameBasedOnEditBuffer(EditBuffer* eb)
{
  auto ebName = eb->getName();

  if(eb->isModified())
    return eb->getParent()->createPresetNameBasedOn(ebName);

  return ebName;
}