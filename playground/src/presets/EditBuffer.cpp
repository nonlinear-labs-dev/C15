#include "EditBuffer.h"
#include "device-settings/DebugLevel.h"
#include "parameters/Parameter.h"
#include "xml/Writer.h"
#include "http/NetworkRequest.h"
#include <http/ContentManager.h>
#include "Application.h"
#include "presets/Preset.h"
#include "presets/PresetBank.h"
#include "presets/PresetManager.h"
#include "EditBufferSnapshotMaker.h"
#include "proxies/lpc/LPCProxy.h"
#include <proxies/hwui/HWUI.h>
#include "parameters/ModulateableParameter.h"
#include <parameters/PhysicalControlParameter.h>
#include "device-settings/Settings.h"
#include "device-settings/RandomizeAmount.h"

shared_ptr<EditBuffer> EditBuffer::createEditBuffer(UpdateDocumentContributor *parent)
{
  EditBuffer *e = new EditBuffer(parent);
  e->init();
  return shared_ptr<EditBuffer>(e);
}

EditBuffer::EditBuffer(UpdateDocumentContributor *parent) :
    Preset(parent),
    m_isModified(false),
    m_deferedJobs(100, std::bind(&EditBuffer::doDeferedJobs, this))
{
  m_selectedParameter = NULL;
  m_hashOnStore = getHash();
}

EditBuffer::~EditBuffer()
{
}

void EditBuffer::resetModifiedIndicator(UNDO::Scope::tTransactionPtr transaction)
{
  auto swap = UNDO::createSwapData(false, getHash());

  transaction->addSimpleCommand([=](UNDO::Command::State)
  {
    bool oldModifiedState = m_isModified;
    swap->swapWith<0>(m_isModified);
    swap->swapWith<1>(m_hashOnStore);

    if(oldModifiedState != m_isModified)
    m_signalModificationState.send (m_isModified);
  });
}

bool EditBuffer::isModified() const
{
  return m_isModified;
}

connection EditBuffer::onModificationStateChanged(slot<void, bool> s)
{
  return m_signalModificationState.connectAndInit(s, m_isModified);
}

connection EditBuffer::onChange(slot<void> s)
{
  return m_signalChange.connect(s);
}

connection EditBuffer::onPresetLoaded(slot<void> s)
{
  return m_signalPresetLoaded.connect(s);
}

connection EditBuffer::onLocksChanged (slot<void> s)
{
  return m_signalLocksChanged.connectAndInit(s);
}

UpdateDocumentContributor::tUpdateID EditBuffer::onChange(uint64_t flags)
{
  m_deferedJobs.trigger();
  m_signalChange.send();

  if(flags & ChangeFlags::LockState)
  {
    m_signalLocksChanged.deferedSend();
  }
  return Preset::onChange(flags);
}

void EditBuffer::doDeferedJobs()
{
  checkModified();
}

void EditBuffer::checkModified()
{
  auto currentHash = getHash();
  bool modified = currentHash != m_hashOnStore;

  if(m_isModified != modified)
  {
    m_isModified = modified;
    m_signalModificationState.send(m_isModified);

    auto cm = dynamic_cast<ContentManager *>(getRoot());

    if(!cm->isSendResponsesScheduled())
      Preset::onChange();
  }
}

PresetManager *EditBuffer::getParent()
{
  return static_cast<PresetManager *>(Preset::getParent());
}

const PresetManager *EditBuffer::getParent() const
{
  return static_cast<const PresetManager *>(Preset::getParent());
}

sigc::connection EditBuffer::onSelectionChanged(slot<void, Parameter *, Parameter *> s)
{
  return m_signalSelectedParameter.connectAndInit(s, nullptr, getSelected());
}

void EditBuffer::undoableSelectParameter(const Glib::ustring &id)
{
  if(auto p = findParameterByID(stoi(id)))
    undoableSelectParameter(p);
}

void EditBuffer::undoableSelectParameter(uint16_t id)
{
  if(auto p = findParameterByID(id))
    undoableSelectParameter(p);
}

void EditBuffer::undoableSelectParameter(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &id)
{
  if(auto p = findParameterByID(stoi(id)))
    undoableSelectParameter(transaction, p);
}

void EditBuffer::setParameter(size_t id, double cpValue)
{
  if(auto p = findParameterByID(id))
  {
    DebugLevel::gassy("EditBuffer::setParameter", id, cpValue);
    Glib::ustring name = UNDO::StringTools::formatString("Set '%0'", p->getGroupAndParameterName());

    if(cpValue == p->getDefaultValue())
      name += " to Default";

    auto scope = getUndoScope().startContinuousTransaction(p, name);
    p->setCPFromWebUI(scope->getTransaction(), cpValue);
    onChange();
  }
}

void EditBuffer::setModulationSource(int src)
{
  if(auto p = dynamic_cast<ModulateableParameter *>(m_selectedParameter))
  {
    auto scope = getUndoScope().startTransaction("Set MC Select for '%0'", p->getLongName());
    p->undoableSelectModSource(scope->getTransaction(), src);
  }
}

void EditBuffer::setModulationAmount(double amount)
{
  if(auto p = dynamic_cast<ModulateableParameter *>(m_selectedParameter))
  {
    auto scope = getUndoScope().startContinuousTransaction(p->getAmountCookie(), "Set MC Amount for '%0'", p->getGroupAndParameterName());
    p->undoableSetModAmount(scope->getTransaction(), amount);
  }
}

bool EditBuffer::hasLocks()
{
  return searchForAnyParameterWithLock() != nullptr;
}

void EditBuffer::undoableSelectParameter(Parameter *p)
{
  if(p != m_selectedParameter)
  {
    auto scope = getUndoScope().startContinuousTransaction(&m_selectedParameter, chrono::hours(1), "Select '%0'",
        p->getGroupAndParameterName());
    undoableSelectParameter(scope->getTransaction(), p);
  }
  else
  {
    auto hwui = Application::get().getHWUI();

    if(hwui->getFocusAndMode().mode == UIMode::Info)
      hwui->undoableSetFocusAndMode(FocusAndMode(UIFocus::Parameters, UIMode::Info));
    else
      hwui->undoableSetFocusAndMode(FocusAndMode(UIFocus::Parameters, UIMode::Select));
  }
}

void EditBuffer::undoableSelectParameter(UNDO::Scope::tTransactionPtr transaction, Parameter *p)
{
  if(p != m_selectedParameter)
  {
    auto swapData = UNDO::createSwapData(p);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      auto oldSelectedParam = m_selectedParameter;

      swapData->swapWith (m_selectedParameter);

      m_signalSelectedParameter.send (oldSelectedParam, m_selectedParameter);

      if (oldSelectedParam)
      oldSelectedParam->onUnselected();

      if (m_selectedParameter)
      m_selectedParameter->onSelected();

      if(!getParent()->isLoading())
      {
        if (auto hwui = Application::get ().getHWUI ())
        {
          hwui->setFocusAndMode (UIFocus::Parameters);
        }
      }

      onChange ();
    });

    if(auto hwui = Application::get().getHWUI())
    {
      hwui->unsetFineMode();
    }
  }
}

Parameter *EditBuffer::getSelected() const
{
  return m_selectedParameter;
}

void EditBuffer::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < ParameterGroupSet::getUpdateIDOfLastChange();

  writer.writeTag("edit-buffer",
      {
        Attribute("selected-parameter", m_selectedParameter ? m_selectedParameter->getID() : 0),
        Attribute("loaded-preset", m_lastLoadedPresetInfo.presetUUID),
        Attribute("loaded-presets-name", m_lastLoadedPresetInfo.presetName),
        Attribute("loaded-presets-bank-name", m_lastLoadedPresetInfo.bankName),
        Attribute("preset-is-zombie", m_lastLoadedPresetInfo.presetDeleted),
        Attribute("is-modified", m_isModified),
        Attribute("hash", getHash()),
        Attribute("changed",changed),
      }, [&]()
      {
        if (changed)
        {
          super::writeDocument (writer, knownRevision);

          for (const tParameterGroupPtr p : getParameterGroups())
          p->writeDocument (writer, knownRevision);
        }
      });
}

void EditBuffer::undoableLoadSelectedPreset()
{
  if(auto bank = getParent()->getSelectedBank())
  {
    if(auto preset = bank->getPreset(bank->getSelectedPreset()))
    {
      if(!isSelectedPresetLoadedAndUnModified())
      {
        undoableLoad(preset);
      }
    }
  }
}

bool EditBuffer::isSelectedPresetLoadedAndUnModified()
{
  if(auto bank = getParent()->getSelectedBank())
  {
    if(auto preset = bank->getPreset(bank->getSelectedPreset()))
    {
      return getUUIDOfLastLoadedPreset() == preset->getUuid() && !isModified();
    }
  }
  return false;
}

void EditBuffer::undoableLoad(shared_ptr<Preset> preset)
{
  UNDO::Scope::tTransactionScopePtr scope = getUndoScope().startTransaction(preset->getUndoTransactionTitle("Load"));
  undoableLoad(scope->getTransaction(), preset);
}

void EditBuffer::undoableLoad(UNDO::Scope::tTransactionPtr transaction, shared_ptr<Preset> preset)
{
  auto lpc = Application::get().getLPCProxy();
  lpc->toggleSuppressParameterChanges(transaction);

  copyFrom(transaction, preset.get(), true);
  undoableSetLoadedPresetInfo(transaction, preset.get());

  if(PresetBank *bank = dynamic_cast<PresetBank *>(preset->getParent()))
  {
    bank->undoableSelectPreset(transaction, preset->getUuid());
    getParent()->undoableSelectBank(transaction, bank->getUuid());
  }

  lpc->toggleSuppressParameterChanges(transaction);
  resetModifiedIndicator(transaction);
}

void EditBuffer::copyFrom(UNDO::Scope::tTransactionPtr transaction, Preset *other, bool ignoreUUIDs)
{
  super::copyFrom(transaction, other, ignoreUUIDs);
  resetModifiedIndicator(transaction);
}

void EditBuffer::undoableSetLoadedPresetInfo(UNDO::Scope::tTransactionPtr transaction, Preset *preset)
{
  LastLoadedPresetInfo info;
  info.presetUUID = preset->getUuid();
  info.presetName = preset->getName();
  info.presetDeleted = false;
  info.bankUUID = preset->getBank()->getUuid();
  info.bankName = preset->getBank()->getName(true);

  auto oldLoadMark = UNDO::createSwapData(info);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    auto oldPreset = getParent()->findPreset(m_lastLoadedPresetInfo.presetUUID);
    oldLoadMark->swapWith<0> (m_lastLoadedPresetInfo);
    auto newPreset = getParent()->findPreset(m_lastLoadedPresetInfo.presetUUID);

    setName(m_lastLoadedPresetInfo.presetName);
    setUuid(m_lastLoadedPresetInfo.presetUUID);

    m_signalPresetLoaded.send();
    onChange ();
  });
}

void EditBuffer::undoableUpdateLoadedPresetInfo(UNDO::Scope::tTransactionPtr transaction)
{
  LastLoadedPresetInfo info;

  if(PresetManager::tPresetPtr preset = getParent()->findPreset(m_lastLoadedPresetInfo.presetUUID))
  {
    info.presetUUID = preset->getUuid();
    info.presetName = preset->getName();
    info.presetDeleted = false;
    info.bankUUID = preset->getBank()->getUuid();
    info.bankName = preset->getBank()->getName(true);
  }
  else
  {
    info = m_lastLoadedPresetInfo;
    info.presetDeleted = true;
  }

  if(m_lastLoadedPresetInfo != info)
  {
    auto oldLoadMark = UNDO::createSwapData(info);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      auto oldPreset = getParent()->findPreset(m_lastLoadedPresetInfo.presetUUID);
      oldLoadMark->swapWith<0> (m_lastLoadedPresetInfo);
      auto newPreset = getParent()->findPreset(m_lastLoadedPresetInfo.presetUUID);

      setName(m_lastLoadedPresetInfo.presetName);
      setUuid(m_lastLoadedPresetInfo.presetUUID);

      m_signalPresetLoaded.send();
      onChange ();
    });
  }
}

void EditBuffer::undoableClear(UNDO::Scope::tTransactionPtr transaction)
{
  auto firstGroup = *(getParameterGroups().begin());
  auto firstParam = *(firstGroup->getParameters().begin());
  undoableSelectParameter(transaction, firstParam);

  for(auto group : getParameterGroups())
  {
    group->undoableClear(transaction);
  }
}

void EditBuffer::undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator)
{
  UNDO::ActionCommand::tAction sendEditBuffer(bind(&EditBuffer::sendToLPC, this));
  transaction->addSimpleCommand(UNDO::ActionCommand::tAction(), sendEditBuffer);

  auto amount = Application::get().getSettings()->getSetting<RandomizeAmount>()->get();

  for(auto group : getParameterGroups())
    group->undoableRandomize(transaction, initiator, amount);

  transaction->addSimpleCommand(sendEditBuffer, UNDO::ActionCommand::tAction());
}

void EditBuffer::undoableInitSound(UNDO::Scope::tTransactionPtr transaction)
{
  UNDO::ActionCommand::tAction sendEditBuffer(bind(&EditBuffer::sendToLPC, this));
  transaction->addSimpleCommand(UNDO::ActionCommand::tAction(), sendEditBuffer);

  for(auto group : getParameterGroups())
    group->undoableClear(transaction);

  ustring initName = "Init";
  ustring emptyUuid;

  auto swap = UNDO::createSwapData(initName, emptyUuid);
  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    auto oldPreset = getParent()->findPreset(m_lastLoadedPresetInfo.presetUUID);
    swap->swapWith<0> (m_lastLoadedPresetInfo.presetName);
    swap->swapWith<1> (m_lastLoadedPresetInfo.presetUUID);
    auto newPreset = getParent()->findPreset(m_lastLoadedPresetInfo.presetUUID);

    m_signalPresetLoaded.send();
    onChange ();
  });

  undoableSetName(transaction, initName);
  transaction->addSimpleCommand(sendEditBuffer, UNDO::ActionCommand::tAction());
}

void EditBuffer::undoableSetDefaultValues(UNDO::Scope::tTransactionPtr transaction, Preset *other)
{
  auto itThis = getParameterGroups().begin();
  auto itOther = other->getParameterGroups().begin();
  auto endThis = getParameterGroups().end();
  auto endOther = other->getParameterGroups().end();

  for(; itThis != endThis && itOther != endOther; (itThis++), (itOther++))
  {
    (*itThis)->undoableSetDefaultValues(transaction, *itOther);
  }
}

UNDO::Scope &EditBuffer::getUndoScope()
{
  return ParameterGroupSet::getUndoScope();
}

Glib::ustring EditBuffer::getUUIDOfLastLoadedPreset() const
{
  return m_lastLoadedPresetInfo.presetUUID;
}

void EditBuffer::undoableImportReaktorPreset(const Glib::ustring &preset)
{
  auto scope = getUndoScope().startTransaction("Import Reaktor Preset");
  undoableImportReaktorPreset(scope->getTransaction(), preset);
}

void EditBuffer::undoableImportReaktorPreset(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &preset)
{
  try
  {
    std::istringstream input(preset);

    if(readReaktorPresetHeader(input))
    {
      for(auto paramIt : getParametersSortedById())
      {
        if(!undoableImportReaktorParameter(transaction, input, paramIt.second))
        {
          break;
        }
      }
    }
  }
  catch(...)
  {
    DebugLevel::error("Could not parse Reaktor preset.");
  }

  Application::get().getLPCProxy()->sendEditBuffer();
}

bool EditBuffer::readReaktorPresetHeader(std::istringstream &input) const
{
  Glib::ustring version;

  if(std::getline(input, version))
  {
    if(stoi(version) == 1)
    {
      Glib::ustring rangeMin;
      std::getline(input, rangeMin);

      Glib::ustring rangeMax;
      std::getline(input, rangeMax);
      return true;
    }
  }

  return false;
}

bool EditBuffer::undoableImportReaktorParameter(UNDO::Scope::tTransactionPtr transaction, std::istringstream &input, Parameter *param)
{
  Glib::ustring paramValue;

  if(std::getline(input, paramValue))
  {
    DebugLevel::warning(paramValue);

    if(dynamic_cast<PhysicalControlParameter *>(param))
      return true;

    param->undoableLoadValue(transaction, paramValue);

    if(auto modParam = dynamic_cast<ModulateableParameter *>(param))
    {
      Glib::ustring packedModulationInfo;

      if(std::getline(input, packedModulationInfo))
      {
        modParam->undoableLoadPackedModulationInfo(transaction, packedModulationInfo);
        return true;
      }
    }
    else
    {
      return true;
    }
  }

  return false;
}

Glib::ustring EditBuffer::exportReaktorPreset()
{
  stringstream str;
  str << "1" << endl;
  str << "65535" << endl;
  str << "-65536" << endl;

  for(auto paramIt : getParametersSortedById())
  {
    paramIt.second->exportReaktorParameter(str);
  }

  return str.str();
}

void EditBuffer::sendToLPC()
{
  Application::get().getLPCProxy()->sendEditBuffer();
}

void EditBuffer::undoableUnlockAllGroups(UNDO::Scope::tTransactionPtr transaction)
{
  for(auto group : getParameterGroups())
    group->undoableUnlock(transaction);
}

void EditBuffer::undoableLockAllGroups(UNDO::Scope::tTransactionPtr transaction)
{
  for(auto group : getParameterGroups())
    group->undoableLock(transaction);
}

void EditBuffer::undoableToggleGroupLock(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &groupId)
{
  if(auto g = getParameterGroupByID(groupId))
    g->undoableToggleLock(transaction);
}

Parameter *EditBuffer::searchForAnyParameterWithLock() const
{
  for(auto group : getParameterGroups())
  {
    for(auto parameter : group->getParameters())
    {
      if(parameter->isLocked())
        return parameter;
    }
  }
  return nullptr;
}
