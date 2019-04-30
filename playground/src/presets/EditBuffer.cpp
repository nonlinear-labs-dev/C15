#include "EditBuffer.h"
#include "device-settings/DebugLevel.h"
#include "parameters/Parameter.h"
#include "xml/Writer.h"
#include "http/NetworkRequest.h"
#include <http/ContentManager.h>
#include "Application.h"
#include <presets/Preset.h>
#include <presets/Bank.h>
#include "presets/PresetManager.h"
#include "EditBufferSnapshotMaker.h"
#include "proxies/lpc/LPCProxy.h"
#include <proxies/hwui/HWUI.h>
#include "parameters/ModulateableParameter.h"
#include <parameters/PhysicalControlParameter.h>
#include <tools/TimeTools.h>
#include <device-settings/DeviceName.h>
#include "device-settings/Settings.h"
#include "device-settings/RandomizeAmount.h"
#include "device-info/DeviceInformation.h"
#include "parameters/MacroControlParameter.h"
#include <libundo/undo/Transaction.h>

EditBuffer::EditBuffer(PresetManager *parent)
    : ParameterGroupSet(parent)
    , m_deferedJobs(100, std::bind(&EditBuffer::doDeferedJobs, this))
    , m_isModified(false)
    , m_recallSet(this)
{
  m_selectedParameter = nullptr;
  m_hashOnStore = getHash();
}

EditBuffer::~EditBuffer()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

void EditBuffer::initRecallValues(UNDO::Transaction *transaction)
{
  initRecallValues(transaction, getParent()->findPreset(m_lastLoadedPreset));
}

void EditBuffer::initRecallValues(UNDO::Transaction *transaction, const Preset *p)
{
  if(p != nullptr)
    m_recallSet.copyParamSet(transaction, p);
  else
    m_recallSet.onPresetDeleted(transaction);
}

const Glib::ustring &EditBuffer::getRecallOrigin() const
{
  return m_recallSet.m_origin;
}

Glib::ustring EditBuffer::getName() const
{
  if(auto o = getOrigin())
    return o->getName();

  return m_name;
}

size_t EditBuffer::getHash() const
{
  size_t hash = AttributesOwner::getHash();

  for(auto g : getParameterGroups())
    hash_combine(hash, g->getHash());

  return hash;
}

const Preset *EditBuffer::getOrigin() const
{
  if(m_originCache && m_originCache->getUuid() == m_lastLoadedPreset)
    return m_originCache;

  m_originCache = getParent()->findPreset(m_lastLoadedPreset);
  return m_originCache;
}

RecallParameterGroups &EditBuffer::getRecallParameterSet()
{
  return m_recallSet;
}

void EditBuffer::resetModifiedIndicator(UNDO::Transaction *transaction)
{
  resetModifiedIndicator(transaction, getHash());
}

void EditBuffer::resetModifiedIndicator(UNDO::Transaction *transaction, size_t hash)
{
  auto swap = UNDO::createSwapData(false, hash);

  transaction->addSimpleCommand([=](UNDO::Command::State) {
    bool oldModifiedState = m_isModified;
    swap->swapWith<0>(m_isModified);
    swap->swapWith<1>(m_hashOnStore);

    if(oldModifiedState != m_isModified)
      m_signalModificationState.send(m_isModified);
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
  return m_signalChange.connectAndInit(s);
}

connection EditBuffer::onPresetLoaded(slot<void> s)
{
  return m_signalPresetLoaded.connect(s);
}

connection EditBuffer::onLocksChanged(slot<void> s)
{
  return m_signalLocksChanged.connectAndInit(s);
}

connection EditBuffer::onRecallValuesChanged(slot<void> s)
{
  return m_recallSet.m_signalRecallValues.connect(s);
}

UpdateDocumentContributor::tUpdateID EditBuffer::onChange(uint64_t flags)
{
  m_deferedJobs.trigger();
  m_signalChange.send();

  if(flags & ChangeFlags::LockState)
  {
    m_signalLocksChanged.deferedSend();
  }

  return ParameterGroupSet::onChange(flags);
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
      onChange();
  }
}

PresetManager *EditBuffer::getParent()
{
  return static_cast<PresetManager *>(super::getParent());
}

const PresetManager *EditBuffer::getParent() const
{
  return static_cast<const PresetManager *>(super::getParent());
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

void EditBuffer::undoableSelectParameter(UNDO::Transaction *transaction, const Glib::ustring &id)
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

void EditBuffer::setModulationSource(ModulationSource src)
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
    auto scope = getUndoScope().startContinuousTransaction(p->getAmountCookie(), "Set MC Amount for '%0'",
                                                           p->getGroupAndParameterName());
    p->undoableSetModAmount(scope->getTransaction(), amount);
  }
}

bool EditBuffer::hasLocks() const
{
  return searchForAnyParameterWithLock() != nullptr;
}

bool EditBuffer::anyParameterChanged() const
{
  for(auto &paramGroup : getParameterGroups())
  {
    for(auto &param : paramGroup->getParameters())
    {
      if(param->isChangedFromLoaded())
      {
        return true;
      }
    }
  }
  return false;
}

void EditBuffer::resetOriginIf(const Preset *p)
{
  if(m_originCache == p)
    m_originCache = nullptr;
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

void EditBuffer::undoableSelectParameter(UNDO::Transaction *transaction, Parameter *p)
{
  if(p != m_selectedParameter)
  {
    auto swapData = UNDO::createSwapData(p);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      auto oldSelectedParam = m_selectedParameter;

      swapData->swapWith(m_selectedParameter);

      m_signalSelectedParameter.send(oldSelectedParam, m_selectedParameter);

      if(oldSelectedParam)
        oldSelectedParam->onUnselected();

      if(m_selectedParameter)
        m_selectedParameter->onSelected();

      if(!getParent()->isLoading())
      {
        if(auto hwui = Application::get().getHWUI())
        {
          hwui->setFocusAndMode(UIFocus::Parameters);
        }
      }

      onChange();
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

void EditBuffer::setName(UNDO::Transaction *transaction, const ustring &name)
{
  transaction->addUndoSwap(this, m_name, name);
}

bool EditBuffer::isZombie() const
{

  if(getUUIDOfLastLoadedPreset() == Uuid::init())
    return false;

  return !getParent()->findPreset(getUUIDOfLastLoadedPreset());
}

void EditBuffer::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  auto changed = knownRevision < ParameterGroupSet::getUpdateIDOfLastChange();
  auto pm = static_cast<const PresetManager *>(getParent());
  auto origin = pm->findPreset(getUUIDOfLastLoadedPreset());
  auto zombie = isZombie();
  auto bank = origin ? dynamic_cast<const Bank *>(origin->getParent()) : nullptr;
  auto bankName = bank ? bank->getName(true) : "";

  writer.writeTag("edit-buffer",
                  { Attribute("selected-parameter", m_selectedParameter ? m_selectedParameter->getID() : 0),
                    Attribute("loaded-preset", getUUIDOfLastLoadedPreset().raw()),
                    Attribute("loaded-presets-name", getName()), Attribute("loaded-presets-bank-name", bankName),
                    Attribute("preset-is-zombie", zombie), Attribute("is-modified", m_isModified),
                    Attribute("hash", getHash()), Attribute("changed", changed) },
                  [&]() {
                    if(changed)
                      super::writeDocument(writer, knownRevision);

                    m_recallSet.writeDocument(writer, knownRevision);
                  });
}

void EditBuffer::undoableLoadSelectedPreset()
{
  if(auto bank = getParent()->getSelectedBank())
  {
    if(auto preset = bank->getSelectedPreset())
    {
      undoableLoad(preset);
    }
  }
}

void EditBuffer::undoableLoad(Preset *preset)
{
  UNDO::Scope::tTransactionScopePtr scope = getUndoScope().startTransaction(preset->buildUndoTransactionTitle("Load"));
  undoableLoad(scope->getTransaction(), preset);
}

void EditBuffer::undoableLoad(UNDO::Transaction *transaction, Preset *preset)
{
  auto lpc = Application::get().getLPCProxy();
  lpc->toggleSuppressParameterChanges(transaction);

  copyFrom(transaction, preset);
  undoableSetLoadedPresetInfo(transaction, preset);

  if(auto bank = static_cast<Bank *>(preset->getParent()))
  {
    auto pm = static_cast<PresetManager *>(getParent());
    bank->selectPreset(transaction, preset->getUuid());
    pm->selectBank(transaction, bank->getUuid());
  }

  lpc->toggleSuppressParameterChanges(transaction);
  resetModifiedIndicator(transaction, getHash());
}

void EditBuffer::copyFrom(UNDO::Transaction *transaction, const Preset *preset)
{
  super::copyFrom(transaction, preset);
  resetModifiedIndicator(transaction, getHash());
}

void EditBuffer::undoableSetLoadedPresetInfo(UNDO::Transaction *transaction, Preset *preset)
{
  Uuid newId = Uuid::none();
  if(preset)
  {
    setName(transaction, preset->getName());
    newId = preset->getUuid();
  }

  auto swap = UNDO::createSwapData(std::move(newId));

  transaction->addSimpleCommand([=](auto) {
    swap->swapWith(m_lastLoadedPreset);
    m_signalPresetLoaded.send();
    m_updateIdWhenLastLoadedPresetChanged = onChange();
  });

  initRecallValues(transaction, preset);
}

void EditBuffer::undoableUpdateLoadedPresetInfo(UNDO::Transaction *transaction)
{
  auto pm = static_cast<PresetManager *>(getParent());
  undoableSetLoadedPresetInfo(transaction, pm->findPreset(getUUIDOfLastLoadedPreset()));
}

void EditBuffer::undoableClear(UNDO::Transaction *transaction)
{
  auto firstGroup = *(getParameterGroups().begin());
  auto firstParam = *(firstGroup->getParameters().begin());
  undoableSelectParameter(transaction, firstParam);

  for(auto group : getParameterGroups())
  {
    group->undoableClear(transaction);
  }
}

void EditBuffer::undoableRandomize(UNDO::Transaction *transaction, Initiator initiator)
{
  UNDO::ActionCommand::tAction sendEditBuffer(bind(&EditBuffer::sendToLPC, this));
  transaction->addSimpleCommand(UNDO::ActionCommand::tAction(), sendEditBuffer);

  auto amount = Application::get().getSettings()->getSetting<RandomizeAmount>()->get();

  for(auto group : getParameterGroups())
    group->undoableRandomize(transaction, initiator, amount);

  transaction->addSimpleCommand(sendEditBuffer, UNDO::ActionCommand::tAction());
}

void EditBuffer::undoableInitSound(UNDO::Transaction *transaction)
{
  UNDO::ActionCommand::tAction sendEditBuffer(bind(&EditBuffer::sendToLPC, this));
  transaction->addSimpleCommand(UNDO::ActionCommand::tAction(), sendEditBuffer);

  for(auto group : getParameterGroups())
    group->undoableClear(transaction);

  auto swap = UNDO::createSwapData(Uuid::init());
  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    swap->swapWith(m_lastLoadedPreset);
    m_signalPresetLoaded.send();
    m_updateIdWhenLastLoadedPresetChanged = onChange();
  });

  resetModifiedIndicator(transaction);

  setName(transaction, "Init Sound");
  m_recallSet.copyFromEditBuffer(transaction, this);

  transaction->addSimpleCommand(sendEditBuffer, UNDO::ActionCommand::tAction());
}

void EditBuffer::undoableSetDefaultValues(UNDO::Transaction *transaction, Preset *other)
{
  for(auto &g : getParameterGroups())
    g->undoableSetDefaultValues(transaction, other->findParameterGroup(g->getID()));
}

UNDO::Scope &EditBuffer::getUndoScope()
{
  return ParameterGroupSet::getUndoScope();
}

Uuid EditBuffer::getUUIDOfLastLoadedPreset() const
{
  return m_lastLoadedPreset;
}

void EditBuffer::undoableImportReaktorPreset(const Glib::ustring &preset)
{
  auto scope = getUndoScope().startTransaction("Import Reaktor Preset");
  undoableImportReaktorPreset(scope->getTransaction(), preset);
}

void EditBuffer::undoableImportReaktorPreset(UNDO::Transaction *transaction, const Glib::ustring &preset)
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

bool EditBuffer::undoableImportReaktorParameter(UNDO::Transaction *transaction, std::istringstream &input,
                                                Parameter *param)
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

void EditBuffer::undoableUnlockAllGroups(UNDO::Transaction *transaction)
{
  for(auto group : getParameterGroups())
    group->undoableUnlock(transaction);
}

void EditBuffer::undoableLockAllGroups(UNDO::Transaction *transaction)
{
  for(auto group : getParameterGroups())
    group->undoableLock(transaction);
}

void EditBuffer::undoableToggleGroupLock(UNDO::Transaction *transaction, const Glib::ustring &groupId)
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

void EditBuffer::setMacroControlValueFromMCView(int id, double value, Glib::ustring uuid)
{
  if(auto mcs = getParameterGroupByID("MCs"))
    if(auto mc = dynamic_cast<MacroControlParameter *>(mcs->getParameterByID(id)))
    {
      mc->setCPFromMCView(mc->getUndoScope().startTrashTransaction()->getTransaction(), value);
      mc->setLastMCViewUUID(uuid);
    }
}
