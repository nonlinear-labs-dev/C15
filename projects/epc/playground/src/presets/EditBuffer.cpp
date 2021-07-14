#include "EditBuffer.h"
#include "device-settings/DebugLevel.h"
#include "parameters/Parameter.h"
#include "xml/Writer.h"
#include <http/ContentManager.h>
#include "Application.h"
#include <presets/Preset.h>
#include <presets/Bank.h>
#include "presets/PresetManager.h"
#include "EditBufferSnapshotMaker.h"
#include "proxies/playcontroller/PlaycontrollerProxy.h"
#include "proxies/hwui/HWUI.h"
#include <proxies/audio-engine/AudioEngineProxy.h>
#include "parameters/ModulateableParameter.h"
#include <parameters/PhysicalControlParameter.h>
#include <device-settings/DeviceName.h>
#include "device-settings/RandomizeAmount.h"
#include "device-info/DeviceInformation.h"
#include "parameters/MacroControlParameter.h"
#include <libundo/undo/Transaction.h>
#include <parameters/AftertouchParameter.h>
#include <parameters/PedalParameter.h>
#include <parameters/RibbonParameter.h>
#include <parameters/PitchbendParameter.h>
#include <parameters/SplitPointParameter.h>
#include <nltools/Testing.h>
#include <nltools/Types.h>
#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>
#include <device-settings/DirectLoadSetting.h>
#include <presets/recall/RecallParameter.h>
#include <parameters/UnisonVoicesParameter.h>
#include <groups/SplitParameterGroups.h>
#include <groups/MonoGroup.h>
#include <groups/UnisonGroup.h>
#include <presets/PresetParameter.h>
#include <tools/PerformanceTimer.h>
#include <device-settings/Settings.h>
#include <parameters/scale-converters/LinearBipolar48StScaleConverter.h>
#include <parameters/ScopedLock.h>
#include <tools/StringTools.h>
#include <parameter_declarations.h>
#include <presets/SendEditBufferScopeGuard.h>
#include <presets/Preset.h>
#include <device-settings/SplitPointSyncParameters.h>
#include <device-settings/SyncSplitSettingUseCases.h>
#include <libundo/undo/ContinuousTransaction.h>
#include "LoadedPresetLog.h"

EditBuffer::EditBuffer(PresetManager *parent)
    : ParameterGroupSet(parent)
    , m_deferredJobs(100, std::bind(&EditBuffer::doDeferedJobs, this))
    , m_isModified(false)
    , m_recallSet(this)
    , m_type(SoundType::Single)
    , m_lastSelectedParameter { 0, VoiceGroup::I }
    , m_loadedPresetLog(std::make_unique<LoadedPresetLog>())
{
  m_hashOnStore = getHash();
}

EditBuffer::~EditBuffer()
{
  m_voiceGroupConnection.disconnect();
}

void EditBuffer::initRecallValues(UNDO::Transaction *transaction)
{
  m_recallSet.copyFromEditBuffer(transaction, this);
}

SoundType EditBuffer::getType() const
{
  return m_type;
}

Glib::ustring EditBuffer::getName() const
{
  return getEditBufferName();
}

Glib::ustring EditBuffer::getEditBufferName() const
{
  if(auto o = getOrigin())
    return o->getName();

  return m_name;
}

size_t EditBuffer::getHash() const
{
  size_t hash = AttributesOwner::getHash();

  for(auto &vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
    for(const auto g : getParameterGroups(vg))
      hash_combine(hash, g->getHash());

  hash_combine(hash, static_cast<int>(getType()));

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
    auto oldState = m_isModified;

    swap->swapWith<0>(m_isModified);
    swap->swapWith<1>(m_hashOnStore);

    if(oldState != m_isModified)
      m_signalModificationState.send(m_isModified);
  });
}

bool EditBuffer::isModified() const
{
  return m_isModified;
}

sigc::connection EditBuffer::onModificationStateChanged(const sigc::slot<void, bool> &s)
{
  return m_signalModificationState.connectAndInit(s, m_isModified);
}

sigc::connection EditBuffer::onChange(const sigc::slot<void> &s, bool init)
{
  if(init)
    return m_signalChange.connectAndInit(s);
  else
    return m_signalChange.connect(s);
}

sigc::connection EditBuffer::onPresetLoaded(const sigc::slot<void> &s)
{
  return m_signalPresetLoaded.connect(s);
}

sigc::connection EditBuffer::onLocksChanged(const sigc::slot<void> &s)
{
  return m_signalLocksChanged.connectAndInit(s);
}

sigc::connection EditBuffer::onRecallValuesChanged(const sigc::slot<void> &s)
{
  return m_recallSet.m_signalRecallValues.connect(s);
}

sigc::connection EditBuffer::onSoundTypeChanged(const sigc::slot<void, SoundType> &s)
{
  return m_signalTypeChanged.connectAndInit(s, m_type);
}

sigc::connection EditBuffer::onSoundTypeChanged(const sigc::slot<void, SoundType> &s, bool init)
{
  if(init)
    return m_signalTypeChanged.connectAndInit(s, m_type);
  else
    return m_signalTypeChanged.connect(s);
}

sigc::connection EditBuffer::onEditBufferConverted(const sigc::slot<void, SoundType> &s)
{
  return m_signalConversionHappened.connect(s);
}

UpdateDocumentContributor::tUpdateID EditBuffer::onChange(uint64_t flags)
{
  m_deferredJobs.trigger();
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
  m_loadedPresetLog->update(m_presetOriginDescription);
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

sigc::connection EditBuffer::onSelectionChanged(const sigc::slot<void, Parameter *, Parameter *> &s,
                                                std::optional<VoiceGroup> initialVG)
{
  if(initialVG.has_value())
  {
    return m_signalSelectedParameter.connectAndInit(s, nullptr, getSelected(initialVG.value()));
  }
  else
  {
    return m_signalSelectedParameter.connect(s);
  }
}

void EditBuffer::undoableSelectParameter(UNDO::Transaction *transaction, const ParameterId &id,
                                         bool sendReselectionSignal)
{
  if(auto p = findParameterByID(id))
    undoableSelectParameter(transaction, p, sendReselectionSignal);
  else
    throw std::runtime_error("could not select parameter: " + id.toString());
}

bool EditBuffer::hasLocks(VoiceGroup vg) const
{
  return searchForAnyParameterWithLock(vg) != nullptr;
}

bool EditBuffer::findAnyParameterChanged(VoiceGroup vg) const
{
  for(auto &paramGroup : getParameterGroups(vg))
    for(auto &param : paramGroup->getParameters())
      if(param->isChangedFromLoaded())
        return true;

  return false;
}

bool EditBuffer::findAnyParameterChanged() const
{
  if(m_type == SoundType::Single)
  {
    return findAnyParameterChanged(VoiceGroup::I) || findAnyParameterChanged(VoiceGroup::Global);
  }
  else
  {
    auto labelChanged = isPartLabelChanged(VoiceGroup::I) || isPartLabelChanged(VoiceGroup::II);

    return labelChanged || findAnyParameterChanged(VoiceGroup::I) || findAnyParameterChanged(VoiceGroup::II)
        || findAnyParameterChanged(VoiceGroup::Global);
  }
}

void EditBuffer::resetOriginIf(const Preset *p)
{
  if(m_originCache == p)
    m_originCache = nullptr;
}

bool EditBuffer::isDual() const
{
  return getType() != SoundType::Single;
}

bool EditBuffer::isParameterFocusLocked() const
{
  return m_lockParameterFocusChanges;
}

void EditBuffer::lockParameterFocusChanges()
{
  m_lockParameterFocusChanges = true;
}

void EditBuffer::unlockParameterFocusChanges()
{
  m_lockParameterFocusChanges = false;
}

void EditBuffer::undoableSelectParameter(UNDO::Transaction *transaction, Parameter *p, bool sendReselectionSignal)
{
  if(m_lastSelectedParameter != p->getID())
  {
    auto swapData = UNDO::createSwapData(p->getID());

    p->resetWasDefaulted(transaction);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      auto oldSelection = m_lastSelectedParameter;

      swapData->swapWith(m_lastSelectedParameter);

      auto oldP = findParameterByID(oldSelection);
      auto newP = findParameterByID(m_lastSelectedParameter);

      m_signalSelectedParameter.send(oldP, newP);

      if(oldP)
        oldP->onUnselected();

      if(newP)
        newP->onSelected();

      onChange();
    });
  }
  else if(sendReselectionSignal)
  {
    transaction->addSimpleCommand([=](auto) mutable { m_signalReselectParameter.send(p); });
  }
}

Parameter *EditBuffer::getSelected(VoiceGroup voiceGroup) const
{
  if(!isDual() && (voiceGroup == VoiceGroup::II))
    voiceGroup = VoiceGroup::I;

  if(ParameterId::isGlobal(m_lastSelectedParameter.getNumber()))
    voiceGroup = VoiceGroup::Global;
  else if(voiceGroup == VoiceGroup::Global)
    voiceGroup = VoiceGroup::I;

  if(getType() == SoundType::Layer)
  {
    static std::vector<C15::PID::ParameterID> layerPTIOnly { C15::PID::Unison_Voices,   C15::PID::Unison_Detune,
                                                             C15::PID::Unison_Pan,      C15::PID::Unison_Phase,
                                                             C15::PID::Mono_Grp_Enable, C15::PID::Mono_Grp_Glide,
                                                             C15::PID::Mono_Grp_Legato, C15::PID::Mono_Grp_Prio };

    if(std::find(layerPTIOnly.begin(), layerPTIOnly.end(), m_lastSelectedParameter.getNumber()) != layerPTIOnly.end())
    {
      voiceGroup = VoiceGroup::I;
    }
  }

  return findParameterByID({ m_lastSelectedParameter.getNumber(), voiceGroup });
}

void EditBuffer::setName(UNDO::Transaction *transaction, const Glib::ustring &name)
{
  transaction->addUndoSwap(this, m_name, name);
}

void EditBuffer::setVoiceGroupName(UNDO::Transaction *transaction, const Glib::ustring &name, VoiceGroup vg)
{
  nltools_assertOnDevPC(vg == VoiceGroup::I || vg == VoiceGroup::II);
  transaction->addUndoSwap(this, m_voiceGroupLabels[static_cast<size_t>(vg)], name);
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
  auto vgIName = getVoiceGroupName(VoiceGroup::I);
  auto vgINameWithSuffix = getVoiceGroupNameWithSuffix(VoiceGroup::I, true);
  auto vgIIName = getVoiceGroupName(VoiceGroup::II);
  auto vgIINameWithSuffix = getVoiceGroupNameWithSuffix(VoiceGroup::II, true);

  writer.writeTag(
      "edit-buffer",
      { Attribute("selected-parameter", m_lastSelectedParameter), Attribute("editbuffer-type", toString(m_type)),
        Attribute("loaded-preset", getUUIDOfLastLoadedPreset().raw()), Attribute("loaded-presets-name", getName()),
        Attribute("loaded-presets-bank-name", bankName), Attribute("preset-is-zombie", zombie),
        Attribute("is-modified", m_isModified), Attribute("hash", getHash()), Attribute("changed", changed),
        Attribute("vg-I-name", vgIName), Attribute("vg-II-name", vgIIName),
        Attribute("vg-I-name-with-suffix", vgINameWithSuffix), Attribute("vg-II-name-with-suffix", vgIINameWithSuffix),
        Attribute("origin-I", getAttribute("origin-I", "")), Attribute("origin-II", getAttribute("origin-II", "")),
        Attribute("origin-I-vg", getAttribute("origin-I-vg", "")),
        Attribute("origin-II-vg", getAttribute("origin-II-vg", "")) },
      [&]() {
        if(changed)
          super::writeDocument(writer, knownRevision);
        m_recallSet.writeDocument(writer, knownRevision);
      });
}

bool isLoadToPartActive()
{
  auto hwui = Application::get().getHWUI();
  return hwui->isInLoadToPart();
}

void EditBuffer::undoableLoadSelectedPreset(UNDO::Transaction *transaction, VoiceGroup loadInto)
{
  if(auto bank = getParent()->getSelectedBank())
  {
    if(auto preset = bank->getSelectedPreset())
    {
      if(isLoadToPartActive() && isDual())
      {
        if(!preset->isDual())
          undoableLoadPresetIntoDualSound(transaction, preset, loadInto);
      }
      else
      {
        undoableLoad(transaction, preset, true);
      }
    }
  }
}

void EditBuffer::undoableLoad(UNDO::Transaction *transaction, const Preset *preset, bool sendToAudioEngine)
{
  if(auto p = dynamic_cast<UNDO::ContinuousTransaction *>(transaction))
    p->stopContinuation();  // if transaction was created for a select operation, direct-load has to stop replacing the transaction

  auto hwui = Application::get().getHWUI();
  auto parameterFocusLock = hwui->getParameterFocusLockGuard();

  PerformanceTimer timer(__PRETTY_FUNCTION__);
  SendEditBufferScopeGuard scope(transaction, sendToAudioEngine);

  const auto oldType = getType();

  setAttribute(transaction, "origin-I", preset->getUuid().raw());
  setAttribute(transaction, "origin-I-vg", toString(VoiceGroup::I));
  setAttribute(transaction, "origin-II", preset->getUuid().raw());
  setAttribute(transaction, "origin-II-vg", toString(VoiceGroup::II));

  copyFrom(transaction, preset);
  undoableSetLoadedPresetInfo(transaction, preset);

  if(preset->getType() == SoundType::Split)
  {
    cleanupSplitPointIfOldPreset(transaction, preset);
  }

  if(auto bank = dynamic_cast<Bank *>(preset->getParent()))
  {
    auto pm = static_cast<PresetManager *>(getParent());
    bank->selectPreset(transaction, preset->getUuid());
    pm->selectBank(transaction, bank->getUuid());
  }

  setSyncSplitSettingAccordingToLoadedPreset(transaction);
  cleanupParameterSelection(transaction, oldType, preset->getType());
  resetModifiedIndicator(transaction, getHash());
}

void EditBuffer::undoableLoadToPart(UNDO::Transaction *trans, const Preset *p, VoiceGroup from, VoiceGroup to)
{
  undoableLoadPresetPartIntoPart(trans, p, from, to);
}

void EditBuffer::copyFrom(UNDO::Transaction *transaction, const Preset *preset)
{
  EditBufferSnapshotMaker::get().addSnapshotIfRequired(transaction, this);
  undoableSetType(transaction, preset->getType());
  super::copyFrom(transaction, preset);
  resetModifiedIndicator(transaction, getHash());
}

void EditBuffer::undoableSetLoadedPresetInfo(UNDO::Transaction *transaction, const Preset *preset)
{
  Uuid newId = Uuid::none();
  std::string presetOriginDescription = "Init";

  if(preset)
  {
    setName(transaction, preset->getName());
    setVoiceGroupName(transaction, preset->getVoiceGroupName(VoiceGroup::I), VoiceGroup::I);
    setVoiceGroupName(transaction, preset->getVoiceGroupName(VoiceGroup::II), VoiceGroup::II);
    newId = preset->getUuid();

    if(auto d = preset->getOriginDescription())
    {
      presetOriginDescription = d.value();
    }
    else
    {
      auto pm = getParent();

      if(auto preset = pm->findPreset(newId))
      {
        if(auto bank = dynamic_cast<Bank *>(preset->getParent()))
        {
          auto pPos = bank->getPresetPosition(preset);
          auto bPos = pm->getBankPosition(bank->getUuid());
          auto bName = bank->getName(true);
          presetOriginDescription
              = StringTools::buildString(bPos + 1, ": ", bName, " / ", pPos + 1, ": ", preset->getName());
        }
        else
        {
          presetOriginDescription = preset->getName();
        }
      }
    }
  }

  auto swap = UNDO::createSwapData(std::move(newId), std::move(presetOriginDescription));

  transaction->addSimpleCommand([=](auto) {
    swap->swapWith<0>(m_lastLoadedPreset);
    swap->swapWith<1>(m_presetOriginDescription);
    m_signalPresetLoaded.send();
    onChange();
  });

  initRecallValues(transaction);
}

void EditBuffer::undoableUpdateLoadedPresetInfo(UNDO::Transaction *transaction)
{
  auto pm = static_cast<PresetManager *>(getParent());
  undoableSetLoadedPresetInfo(transaction, pm->findPreset(getUUIDOfLastLoadedPreset()));
}

void EditBuffer::undoableRandomize(UNDO::Transaction *transaction, Initiator initiator)
{
  SendEditBufferScopeGuard scopeGuard(transaction, true);

  auto amount = Application::get().getSettings()->getSetting<RandomizeAmount>()->get();

  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    for(auto &group : getParameterGroups(vg))
      group->undoableRandomize(transaction, initiator, amount);
}

void EditBuffer::undoableRandomizePart(UNDO::Transaction *transaction, VoiceGroup vg, Initiator initiator)
{
  SendEditBufferScopeGuard scopeGuard(transaction, true);

  auto amount = Application::get().getSettings()->getSetting<RandomizeAmount>()->get();

  for(auto &g : getParameterGroups(vg))
    g->undoableRandomize(transaction, initiator, amount);
}

void EditBuffer::undoableInitSound(UNDO::Transaction *transaction, Defaults mode)
{
  SendEditBufferScopeGuard scopeGuard(transaction, true);

  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    undoableInitPart(transaction, vg, mode);

  auto swap = UNDO::createSwapData(Uuid::init());
  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    swap->swapWith(m_lastLoadedPreset);
    m_signalPresetLoaded.send();
    onChange();
  });

  resetModifiedIndicator(transaction);

  setName(transaction, "Init Sound");
  setAttribute(transaction, "origin-I", "");
  setAttribute(transaction, "origin-II", "");
  setAttribute(transaction, "origin-I-vg", "");
  setAttribute(transaction, "origin-II-vg", "");

  m_recallSet.copyFromEditBuffer(transaction, this);
}

void EditBuffer::undoableSetDefaultValues(UNDO::Transaction *transaction, Preset *other)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    for(auto &g : getParameterGroups(vg))
      if(auto otherGroup = other->findParameterGroup(g->getID()))
        g->undoableSetDefaultValues(transaction, otherGroup);
}

UNDO::Scope &EditBuffer::getUndoScope()
{
  return ParameterGroupSet::getUndoScope();
}

Uuid EditBuffer::getUUIDOfLastLoadedPreset() const
{
  return m_lastLoadedPreset;
}

void EditBuffer::sendToAudioEngine()
{
  Application::get().getAudioEngineProxy()->sendEditBuffer();
}

void EditBuffer::undoableUnlockAllGroups(UNDO::Transaction *transaction)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    for(auto group : getParameterGroups(vg))
      group->undoableUnlock(transaction);
}

void EditBuffer::undoableLockAllGroups(UNDO::Transaction *transaction)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    for(auto group : getParameterGroups(vg))
      group->undoableLock(transaction);
}

void EditBuffer::undoableToggleGroupLock(UNDO::Transaction *transaction, const Glib::ustring &groupId)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    if(auto g = getParameterGroupByID({ groupId, vg }))
      g->undoableToggleLock(transaction);
}

Parameter *EditBuffer::searchForAnyParameterWithLock(VoiceGroup vg) const
{
  for(auto group : getParameterGroups(vg))
  {
    for(auto parameter : group->getParameters())
    {
      if(parameter->isLocked())
        return parameter;
    }
  }
  return nullptr;
}

void EditBuffer::setMacroControlValueFromMCView(ParameterId id, double value, const Glib::ustring &uuid)
{
  if(auto mc = dynamic_cast<MacroControlParameter *>(findParameterByID(id)))
  {
    mc->setCPFromMCView(mc->getUndoScope().startTrashTransaction()->getTransaction(), value);
    mc->setLastMCViewUUID(uuid);
  }
}

VoiceGroup invert(VoiceGroup vg)
{
  return vg == VoiceGroup::I ? VoiceGroup::II : VoiceGroup::I;
}

void EditBuffer::undoableConvertToSingle(UNDO::Transaction *transaction, VoiceGroup copyFrom)
{
  if(getType() != SoundType::Single)
    undoableConvertDualToSingle(transaction, copyFrom);
}

void EditBuffer::combineSplitPartGlobalMaster(UNDO::Transaction *transaction, VoiceGroup copyFrom)
{
  auto masterGroup = getParameterGroupByID({ "Master", VoiceGroup::Global });

  auto originVolume = findAndCastParameterByID<ModulateableParameter>({ C15::PID::Voice_Grp_Volume, copyFrom });
  auto originTune = findAndCastParameterByID<ModulateableParameter>({ C15::PID::Voice_Grp_Tune, copyFrom });

  auto masterVolumeParameter
      = masterGroup->findAndCastParameterByID<ModulateableParameter>({ C15::PID::Master_Volume, VoiceGroup::Global });
  auto masterTuneParameter
      = masterGroup->findAndCastParameterByID<ModulateableParameter>({ C15::PID::Master_Tune, VoiceGroup::Global });

  // unmute both parts
  findParameterByID({ 395, VoiceGroup::I })->setCPFromHwui(transaction, 0);
  findParameterByID({ 395, VoiceGroup::II })->setCPFromHwui(transaction, 0);

  ParabolicGainDbScaleConverter dbGainConverter;

  auto vgVolumeDisplay = dbGainConverter.controlPositionToDisplay(originVolume->getControlPositionValue());
  auto masterVolumeDisplay = dbGainConverter.controlPositionToDisplay(masterVolumeParameter->getControlPositionValue());

  auto newVolume = dbGainConverter.displayToControlPosition(vgVolumeDisplay + masterVolumeDisplay);
  auto newTune = originTune->getControlPositionValue() + masterTuneParameter->getControlPositionValue();

  masterVolumeParameter->copyFrom(transaction, originVolume);
  masterVolumeParameter->setCPFromHwui(transaction, newVolume);

  masterTuneParameter->copyFrom(transaction, originTune);
  masterTuneParameter->setCPFromHwui(transaction, newTune);
}

void EditBuffer::combineLayerPartGlobalMaster(UNDO::Transaction *transaction, VoiceGroup copyFrom)
{
  auto masterGroup = getParameterGroupByID({ "Master", VoiceGroup::Global });

  auto originVolume
      = dynamic_cast<ModulateableParameter *>(findParameterByID({ C15::PID::Voice_Grp_Volume, copyFrom }));
  auto originTune = dynamic_cast<ModulateableParameter *>(findParameterByID({ C15::PID::Voice_Grp_Tune, copyFrom }));

  auto masterVolumeParameter = dynamic_cast<ModulateableParameter *>(
      masterGroup->getParameterByID({ C15::PID::Master_Volume, VoiceGroup::Global }));
  auto masterTuneParameter = dynamic_cast<ModulateableParameter *>(
      masterGroup->getParameterByID({ C15::PID::Master_Tune, VoiceGroup::Global }));

  // unmute both parts
  findParameterByID({ C15::PID::Voice_Grp_Mute, VoiceGroup::I })->setCPFromHwui(transaction, 0);
  findParameterByID({ C15::PID::Voice_Grp_Mute, VoiceGroup::II })->setCPFromHwui(transaction, 0);

  ParabolicGainDbScaleConverter dbGainConverter;

  auto vgVolumeDisplay = dbGainConverter.controlPositionToDisplay(originVolume->getControlPositionValue());
  auto masterVolumeDisplay = dbGainConverter.controlPositionToDisplay(masterVolumeParameter->getControlPositionValue());

  auto newVolume = dbGainConverter.displayToControlPosition(vgVolumeDisplay + masterVolumeDisplay);
  auto newTune = originTune->getControlPositionValue() + masterTuneParameter->getControlPositionValue();

  masterVolumeParameter->setCPFromHwui(transaction, newVolume);
  masterVolumeParameter->setModulationSource(transaction, originVolume->getModulationSource());
  masterVolumeParameter->setModulationAmount(transaction, originVolume->getModulationAmount());

  masterTuneParameter->setCPFromHwui(transaction, newTune);
  masterTuneParameter->setModulationSource(transaction, originTune->getModulationSource());
  masterTuneParameter->setModulationAmount(transaction, originTune->getModulationAmount());
}

void EditBuffer::undoableConvertDualToSingle(UNDO::Transaction *transaction, VoiceGroup copyFrom)
{
  const auto oldType = getType();

  SendEditBufferScopeGuard scopeGuard(transaction, true);

  setName(transaction, getVoiceGroupName(copyFrom));
  undoableSetTypeFromConvert(transaction, SoundType::Single);

  if(oldType == SoundType::Split)
    undoableConvertSplitToSingle(transaction, copyFrom);
  else if(oldType == SoundType::Layer)
    undoableConvertLayerToSingle(transaction, copyFrom);

  initToFX(transaction);
  initFadeFrom(transaction, VoiceGroup::I);
  initFadeFrom(transaction, VoiceGroup::II);
  initCrossFB(transaction);
  initSplitPoint(transaction);

  forEachParameter(VoiceGroup::II, [&](Parameter *p) { p->loadDefault(transaction, Defaults::FactoryDefault); });

  auto vgVolume = findParameterByID({ C15::PID::Voice_Grp_Volume, VoiceGroup::I });
  auto vgTune = findParameterByID({ C15::PID::Voice_Grp_Tune, VoiceGroup::I });
  vgVolume->loadDefault(transaction, Defaults::FactoryDefault);
  vgTune->loadDefault(transaction, Defaults::FactoryDefault);

  transaction->addUndoSwap(this, m_lastLoadedPreset, Uuid::converted());
  setVoiceGroupName(transaction, "", VoiceGroup::I);
  setVoiceGroupName(transaction, "", VoiceGroup::II);

  initRecallValues(transaction);
}

void EditBuffer::undoableConvertLayerToSingle(UNDO::Transaction *transaction, VoiceGroup copyFrom)
{
  combineLayerPartGlobalMaster(transaction, copyFrom);
  ScopedLock locks(transaction);
  locks.addGroupLock({ "Unison", VoiceGroup::I });
  locks.addGroupLock({ "Mono", VoiceGroup::I });
  if(copyFrom != VoiceGroup::I)
    copyVoiceGroup(transaction, copyFrom, VoiceGroup::I);

  if(!StringTools::hasEnding(getVoiceGroupName(copyFrom), "conv."))
  {
    setName(transaction, getVoiceGroupName(copyFrom) + " conv.");
  }
}

void EditBuffer::undoableConvertSplitToSingle(UNDO::Transaction *transaction, VoiceGroup copyFrom)
{
  combineSplitPartGlobalMaster(transaction, copyFrom);
  if(copyFrom != VoiceGroup::I)
    copyVoiceGroup(transaction, copyFrom, VoiceGroup::I);

  if(!StringTools::hasEnding(getVoiceGroupName(copyFrom), "conv."))
  {
    setName(transaction, getVoiceGroupName(copyFrom) + " conv.");
  }
}

void EditBuffer::undoableConvertToDual(UNDO::Transaction *transaction, SoundType type)
{
  const auto oldType = m_type;

  if(oldType == type)
    return;

  SendEditBufferScopeGuard scopeGuard(transaction, true);

  undoableSetTypeFromConvert(transaction, type);

  if(oldType == SoundType::Single && type == SoundType::Layer)
    undoableConvertSingleToLayer(transaction);
  else if(oldType == SoundType::Single && type == SoundType::Split)
    undoableConvertSingleToSplit(transaction);
  else if(oldType == SoundType::Layer && type == SoundType::Split)
    undoableConvertLayerToSplit(transaction);
  else if(oldType == SoundType::Split && type == SoundType::Layer)
    undoableConvertSplitToLayer(transaction);

  initCrossFB(transaction);
  undoableUnmuteLayers(transaction);

  initRecallValues(transaction);
  transaction->addUndoSwap(this, m_lastLoadedPreset, Uuid::converted());
}

void EditBuffer::undoableUnisonMonoLoadDefaults(UNDO::Transaction *transaction, VoiceGroup vg)
{
  getParameterGroupByID({ "Mono", vg })->forEachParameter([&](auto p) -> void {
    p->loadDefault(transaction, Defaults::FactoryDefault);
  });
  getParameterGroupByID({ "Unison", vg })->forEachParameter([&](auto p) -> void {
    p->loadDefault(transaction, Defaults::FactoryDefault);
  });
}

void EditBuffer::undoableUnmuteLayers(UNDO::Transaction *transaction)
{
  findParameterByID({ C15::PID::Voice_Grp_Mute, VoiceGroup::I })->setCPFromHwui(transaction, 0);
  findParameterByID({ C15::PID::Voice_Grp_Mute, VoiceGroup::II })->setCPFromHwui(transaction, 0);
}

void EditBuffer::copyAndInitGlobalMasterGroupToPartMasterGroups(UNDO::Transaction *transaction)
{
  auto global = getParameterGroupByID({ "Master", VoiceGroup::Global });
  auto partI = getParameterGroupByID({ "Part", VoiceGroup::I });
  auto partII = getParameterGroupByID({ "Part", VoiceGroup::II });

  //Copy Volume and Tune
  for(auto &ids : std::vector<std::pair<int, int>> { { C15::PID::Voice_Grp_Volume, C15::PID::Master_Volume },
                                                     { C15::PID::Voice_Grp_Tune, C15::PID::Master_Tune } })
  {
    auto pI = partI->findParameterByID({ ids.first, VoiceGroup::I });
    auto pII = partII->findParameterByID({ ids.first, VoiceGroup::II });
    auto pGlobal = global->findParameterByID({ ids.second, VoiceGroup::Global });

    pI->copyFrom(transaction, pGlobal);
    pII->copyFrom(transaction, pGlobal);
  }

  for(auto &parameter : global->getParameters())
  {
    parameter->loadDefault(transaction, Defaults::FactoryDefault);
  }
}

void EditBuffer::undoableSetType(UNDO::Transaction *transaction, SoundType type)
{
  if(m_type != type)
  {
    auto swap = UNDO::createSwapData(type);

    cleanupParameterSelection(transaction, m_type, type);

    transaction->addSimpleCommand([=](auto state) {
      swap->swapWith(m_type);
      initUnisonVoicesScaling(m_type);
      m_signalTypeChanged.send(m_type);
      onChange();
    });
  }
}

void EditBuffer::undoableLoadPresetIntoDualSound(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup vg)
{
  undoableLoadSinglePresetIntoDualSound(transaction, preset, vg);
}

void EditBuffer::undoableLoadSinglePresetIntoDualSound(UNDO::Transaction *transaction, const Preset *preset,
                                                       VoiceGroup to)
{
  SendEditBufferScopeGuard scopeGuard(transaction, true);
  setVoiceGroupName(transaction, preset->getName(), to);

  {
    ScopedLock lock(transaction);

    lock.addGroupLock({ "Unison", VoiceGroup::I });
    lock.addGroupLock({ "Unison", VoiceGroup::II });
    lock.addGroupLock({ "Mono", VoiceGroup::I });
    lock.addGroupLock({ "Mono", VoiceGroup::II });

    for(auto &p : getCrossFBParameters(VoiceGroup::I))
      lock.addLock(p);

    for(auto &p : getCrossFBParameters(VoiceGroup::II))
      lock.addLock(p);

    loadSinglePresetIntoVoiceGroup(transaction, preset, to);
  }

  loadPresetGlobalMasterIntoVoiceGroupMaster(transaction, preset, to);
  initRecallValues(transaction);
}

Glib::ustring EditBuffer::getVoiceGroupName(VoiceGroup vg) const
{
  nltools_assertOnDevPC(vg == VoiceGroup::I || vg == VoiceGroup::II);
  return m_voiceGroupLabels[static_cast<size_t>(vg)];
}

Glib::ustring EditBuffer::getVoiceGroupNameWithSuffix(VoiceGroup vg, bool addSpace) const
{
  auto monoUnisonVoiceGroup = vg;
  auto isLayer = getType() == SoundType::Layer;
  if(isLayer)
    monoUnisonVoiceGroup = VoiceGroup::I;
  bool mono = isMonoEnabled(monoUnisonVoiceGroup);
  bool unison = hasMoreThanOneUnisonVoice(monoUnisonVoiceGroup);

  return getVoiceGroupName(vg) + (addSpace ? "\u202F" : "") + (mono ? "\uE040" : "") + (unison ? "\uE041" : "");
}

bool EditBuffer::hasMoreThanOneUnisonVoice(const VoiceGroup &vg) const
{
  return findParameterByID({ C15::PID::Unison_Voices, vg })->getControlPositionValue() > 0;
}

bool EditBuffer::isMonoEnabled(const VoiceGroup &vg) const
{
  return findParameterByID({ C15::PID::Mono_Grp_Enable, vg })->getControlPositionValue() > 0;
}

Glib::ustring EditBuffer::getNameWithSuffix() const
{
  auto hasMono = false;
  auto hasUnison = false;
  switch(getType())
  {
    case SoundType::Layer:
    case SoundType::Single:
      hasMono |= isMonoEnabled(VoiceGroup::I);
      hasUnison |= hasMoreThanOneUnisonVoice(VoiceGroup::I);
      break;
    case SoundType::Split:
      hasMono |= isMonoEnabled(VoiceGroup::I);
      hasMono |= isMonoEnabled(VoiceGroup::II);
      hasUnison |= hasMoreThanOneUnisonVoice(VoiceGroup::I);
      hasUnison |= hasMoreThanOneUnisonVoice(VoiceGroup::II);
      break;
    default:
      break;
  }

  return getName() + " " + (hasMono ? "\uE040" : "") + (hasUnison ? "\uE041" : "");
}

void EditBuffer::undoableLoadPresetPartIntoPart(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup from,
                                                VoiceGroup copyTo)
{
  if(!preset)
    return;

  if(preset->getType() == SoundType::Single)
  {
    from = VoiceGroup::I;
  }

  SendEditBufferScopeGuard scopeGuard(transaction, true);

  switch(getType())
  {
    case SoundType::Single:
    case SoundType::Invalid:
      undoableLoadPresetPartIntoSingleSound(transaction, preset, from, VoiceGroup::I);
      break;
    case SoundType::Split:
      if(preset->isDual())
        undoableLoadPresetPartIntoSplitSound(transaction, preset, from, copyTo);
      else
        loadSinglePresetIntoSplitPart(transaction, preset, copyTo);
      break;
    case SoundType::Layer:
      if(preset->isDual())
        undoableLoadPresetPartIntoLayerSound(transaction, preset, from, copyTo);
      else
        loadSinglePresetIntoLayerPart(transaction, preset, copyTo);
      break;
  }
}

void EditBuffer::initUnisonVoicesScaling(SoundType newType)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
    if(auto unisonParam = dynamic_cast<UnisonVoicesParameter *>(findParameterByID({ C15::PID::Unison_Voices, vg })))
      unisonParam->updateScaling(newType);
}

bool EditBuffer::isDualParameterForSoundType(const Parameter *parameter, SoundType type)
{
  auto selectedIsNotGlobal = parameter->getVoiceGroup() != VoiceGroup::Global;

  if(type == SoundType::Single)
  {
    return false;
  }

  if(type == SoundType::Layer)
  {
    if(UnisonGroup::isUnisonParameter(parameter))
      return false;

    if(MonoGroup::isMonoParameter(parameter))
      return false;

    return SplitParameterGroups::isSplitPoint(parameter) || selectedIsNotGlobal;
  }

  if(type == SoundType::Split)
  {
    return SplitParameterGroups::isSplitPoint(parameter) || selectedIsNotGlobal;
  }

  return false;
}

void EditBuffer::undoableInitPart(UNDO::Transaction *transaction, VoiceGroup vg, Defaults mode)
{
  SendEditBufferScopeGuard scopeGuard(transaction, true);

  for(auto &group : getParameterGroups(vg))
    group->undoableLoadDefault(transaction, mode);

  if(vg != VoiceGroup::Global)
  {
    setVoiceGroupName(transaction, "Init", vg);
    setAttribute(transaction, vg == VoiceGroup::I ? "origin-I" : "origin-II", "");
    setAttribute(transaction, vg == VoiceGroup::I ? "origin-I-vg" : "origin-II-vg", "");
  }

  m_recallSet.copyFromEditBuffer(transaction, this, vg);
}

void EditBuffer::TEST_doDeferredJobs()
{
  doDeferedJobs();
}

void EditBuffer::initToFX(UNDO::Transaction *transaction)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
    findParameterByID({ C15::PID::Out_Mix_To_FX, vg })->loadDefault(transaction, Defaults::FactoryDefault);
}

std::vector<Parameter *> EditBuffer::getCrossFBParameters(const VoiceGroup &to) const
{
  return std::vector<Parameter *>(
      { findParameterByID({ C15::PID::FB_Mix_Osc, to }), findParameterByID({ C15::PID::FB_Mix_Osc_Src, to }),
        findParameterByID({ C15::PID::FB_Mix_Comb_Src, to }), findParameterByID({ C15::PID::FB_Mix_SVF_Src, to }),
        findParameterByID({ C15::PID::FB_Mix_FX_Src, to }) });
}

void EditBuffer::loadPresetGlobalMasterIntoVoiceGroupMaster(UNDO::Transaction *transaction, const Preset *preset,
                                                            VoiceGroup copyTo)
{
  auto part = getParameterGroupByID({ "Part", copyTo });

  for(auto &ids : std::vector<std::pair<int, int>> { { C15::PID::Voice_Grp_Volume, C15::PID::Master_Volume },
                                                     { C15::PID::Voice_Grp_Tune, C15::PID::Master_Tune } })
  {
    auto p = part->findParameterByID({ ids.first, part->getVoiceGroup() });
    if(auto pGlobal = preset->findParameterByID({ ids.second, VoiceGroup::Global }, false))
      p->copyFrom(transaction, pGlobal);
  }
}

void EditBuffer::copySumOfMasterGroupToVoiceGroupMasterGroup(UNDO::Transaction *transaction, const Preset *preset,
                                                             VoiceGroup copyFrom, VoiceGroup copyTo)
{
  auto presetGlobalVolume = preset->findParameterByID({ C15::PID::Master_Volume, VoiceGroup::Global }, false);
  auto presetGlobalTune = preset->findParameterByID({ C15::PID::Master_Tune, VoiceGroup::Global }, false);

  auto presetPartVolume = preset->findParameterByID({ C15::PID::Voice_Grp_Volume, copyFrom }, false);
  auto presetPartTune = preset->findParameterByID({ C15::PID::Voice_Grp_Tune, copyFrom }, false);

  auto globalVolume = findParameterByID({ C15::PID::Master_Volume, VoiceGroup::Global });
  auto globalTune = findParameterByID({ C15::PID::Master_Tune, VoiceGroup::Global });

  auto partVolume = dynamic_cast<ModulateableParameter *>(findParameterByID({ C15::PID::Voice_Grp_Volume, copyTo }));
  auto partTune = dynamic_cast<ModulateableParameter *>(findParameterByID({ C15::PID::Voice_Grp_Tune, copyTo }));

  if(presetGlobalVolume && presetGlobalTune && partVolume && partTune)
  {
    auto volumeScaleConverter
        = dynamic_cast<const ParabolicGainDbScaleConverter *>(partVolume->getValue().getScaleConverter());

    auto ebGlobalVolumeDV = volumeScaleConverter->controlPositionToDisplay(globalVolume->getControlPositionValue());
    auto presetGlobalVolumeDV = volumeScaleConverter->controlPositionToDisplay(presetGlobalVolume->getValue());
    auto presetPartVolumeDV = volumeScaleConverter->controlPositionToDisplay(
        presetPartVolume ? presetPartVolume->getValue() : partVolume->getDefaultValue());

    auto newVolumeDV = (presetGlobalVolumeDV + presetPartVolumeDV) - ebGlobalVolumeDV;
    auto newVolumeCP = volumeScaleConverter->displayToControlPosition(newVolumeDV);
    partVolume->setCPFromHwui(transaction, newVolumeCP);

    auto ebGlobalTuneCP = globalTune->getControlPositionValue();

    partTune->setCPFromHwui(
        transaction,
        (presetGlobalTune->getValue() + (presetPartTune ? presetPartTune->getValue() : partTune->getDefaultValue()))
            - ebGlobalTuneCP);

    if(preset->getType() == SoundType::Single)
    {
      partTune->setModulationAmount(transaction, presetGlobalTune->getModulationAmount());
      partTune->setModulationSource(transaction, presetGlobalTune->getModulationSource());

      partVolume->setModulationSource(transaction, presetGlobalVolume->getModulationSource());
      partVolume->setModulationAmount(transaction, presetGlobalVolume->getModulationAmount());
    }
  }
  else if(partTune && partVolume)
  {
    partTune->loadDefault(transaction, Defaults::FactoryDefault);
    partVolume->loadDefault(transaction, Defaults::FactoryDefault);
  }
}

void EditBuffer::copySinglePresetMasterToPartMaster(UNDO::Transaction *transaction, const Preset *preset,
                                                    VoiceGroup targetGroup)
{
  auto presetGlobalVolume = preset->findParameterByID({ C15::PID::Master_Volume, VoiceGroup::Global }, false);
  auto presetGlobalTune = preset->findParameterByID({ C15::PID::Master_Tune, VoiceGroup::Global }, false);

  auto ebGlobalVolume = findParameterByID({ C15::PID::Master_Volume, VoiceGroup::Global });
  auto ebGlobalTune = findParameterByID({ C15::PID::Master_Tune, VoiceGroup::Global });

  auto partVolume
      = dynamic_cast<ModulateableParameter *>(findParameterByID({ C15::PID::Voice_Grp_Volume, targetGroup }));
  auto partTune = dynamic_cast<ModulateableParameter *>(findParameterByID({ C15::PID::Voice_Grp_Tune, targetGroup }));

  if(presetGlobalTune && presetGlobalVolume)
  {
    partTune->setCPFromHwui(transaction, presetGlobalTune->getValue() - ebGlobalTune->getControlPositionValue());
    partTune->setModulationSource(transaction, presetGlobalTune->getModulationSource());
    partTune->setModulationAmount(transaction, presetGlobalTune->getModulationAmount());

    auto volumeConverter
        = static_cast<const ParabolicGainDbScaleConverter *>(ebGlobalVolume->getValue().getScaleConverter());

    auto presetVolumeDisplay = volumeConverter->controlPositionToDisplay(presetGlobalVolume->getValue());
    auto ebVolumeDisplay = volumeConverter->controlPositionToDisplay(ebGlobalVolume->getControlPositionValue());

    auto newVolumeCP = volumeConverter->displayToControlPosition(presetVolumeDisplay - ebVolumeDisplay);

    partVolume->setCPFromHwui(transaction, newVolumeCP);
    partVolume->setModulationSource(transaction, presetGlobalVolume->getModulationSource());
    partVolume->setModulationAmount(transaction, presetGlobalVolume->getModulationAmount());
  }
  else
  {
    partTune->loadDefault(transaction, Defaults::FactoryDefault);
    partVolume->loadDefault(transaction, Defaults::FactoryDefault);
  }
}

void EditBuffer::initSplitPoint(UNDO::Transaction *transaction)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    auto splitPoint = findParameterByID({ C15::PID::Split_Split_Point, vg });
    splitPoint->loadDefault(transaction, Defaults::FactoryDefault);
  }
}

void EditBuffer::initFadeFrom(UNDO::Transaction *transaction, VoiceGroup vg)
{
  findParameterByID({ C15::PID::Voice_Grp_Fade_From, vg })->loadDefault(transaction, Defaults::FactoryDefault);
  findParameterByID({ C15::PID::Voice_Grp_Fade_Range, vg })->loadDefault(transaction, Defaults::FactoryDefault);
}

EditBuffer::PartOrigin EditBuffer::getPartOrigin(VoiceGroup vg) const
{
  PartOrigin ret;

  if(vg == VoiceGroup::I)
  {
    ret.presetUUID = Uuid { getAttribute("origin-I", "") };
    ret.sourceGroup = to<VoiceGroup>(getAttribute("origin-I-vg", "Global"));
  }
  else if(vg == VoiceGroup::II)
  {
    ret.presetUUID = Uuid { getAttribute("origin-II", "") };
    ret.sourceGroup = to<VoiceGroup>(getAttribute("origin-II-vg", "Global"));
  }

  return ret;
}

void EditBuffer::defaultFadeParameters(UNDO::Transaction *transaction)
{
  for(auto &vg : { VoiceGroup::I, VoiceGroup::II })
  {
    findParameterByID({ C15::PID::Voice_Grp_Fade_From, vg })->loadDefault(transaction, Defaults::FactoryDefault);
    findParameterByID({ C15::PID::Voice_Grp_Fade_Range, vg })->loadDefault(transaction, Defaults::FactoryDefault);
  }
}

void EditBuffer::copyVoicesGroups(UNDO::Transaction *transaction, VoiceGroup from, VoiceGroup to)
{
  for(auto &gid : { "Unison", "Mono" })
  {
    for(auto &target : getParameterGroupByID({ gid, to })->getParameters())
    {
      auto src = findParameterByID({ target->getID().getNumber(), from });
      target->copyFrom(transaction, src);
    }
  }
}

void EditBuffer::initFadeParameters(UNDO::Transaction *transaction, VoiceGroup group)
{
  auto f1 = findParameterByID({ C15::PID::Voice_Grp_Fade_From, group });
  if(!f1->isLocked())
    f1->loadDefault(transaction, Defaults::FactoryDefault);

  auto f2 = findParameterByID({ C15::PID::Voice_Grp_Fade_Range, group });
  if(!f2->isLocked())
    f2->loadDefault(transaction, Defaults::FactoryDefault);
}

void EditBuffer::initCrossFB(UNDO::Transaction *transaction)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    for(auto param : getCrossFBParameters(vg))
    {
      param->loadDefault(transaction, Defaults::FactoryDefault);
    }
  }
}

void EditBuffer::undoableConvertSingleToSplit(UNDO::Transaction *transaction)
{
  setVoiceGroupName(transaction, getName(), VoiceGroup::I);
  setVoiceGroupName(transaction, getName(), VoiceGroup::II);
  initToFX(transaction);
  copyVoiceGroup(transaction, VoiceGroup::I, VoiceGroup::II);
  copyAndInitGlobalMasterGroupToPartMasterGroups(transaction);
  initFadeFrom(transaction, VoiceGroup::I);
  initFadeFrom(transaction, VoiceGroup::II);
  initSplitPoint(transaction);
}

void EditBuffer::undoableConvertSingleToLayer(UNDO::Transaction *transaction)
{
  setVoiceGroupName(transaction, getName(), VoiceGroup::I);
  setVoiceGroupName(transaction, getName(), VoiceGroup::II);
  initToFX(transaction);
  copyVoiceGroup(transaction, VoiceGroup::I, VoiceGroup::II);
  undoableUnisonMonoLoadDefaults(transaction, VoiceGroup::II);
  copyAndInitGlobalMasterGroupToPartMasterGroups(transaction);
  initFadeFrom(transaction, VoiceGroup::I);
  initFadeFrom(transaction, VoiceGroup::II);
  undoableUnisonMonoLoadDefaults(transaction, VoiceGroup::II);
  initSplitPoint(transaction);
}

void EditBuffer::undoableConvertLayerToSplit(UNDO::Transaction *transaction)
{
  copyVoicesGroups(transaction, VoiceGroup::I, VoiceGroup::II);
  calculateSplitPointFromFadeParams(transaction);
  initFadeFrom(transaction, VoiceGroup::I);
  initFadeFrom(transaction, VoiceGroup::II);
}

void EditBuffer::undoableConvertSplitToLayer(UNDO::Transaction *transaction)
{
  auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
  copyVoicesGroups(transaction, currentVG, invert(currentVG));
  defaultFadeParameters(transaction);
  undoableUnisonMonoLoadDefaults(transaction, VoiceGroup::II);
  initSplitPoint(transaction);
}

void EditBuffer::calculateSplitPointFromFadeParams(UNDO::Transaction *transaction)
{
  const auto f1 = findParameterByID({ C15::PID::Voice_Grp_Fade_From, VoiceGroup::I })->getControlPositionValue();
  const auto f2 = findParameterByID({ C15::PID::Voice_Grp_Fade_From, VoiceGroup::II })->getControlPositionValue();
  const auto meanFadeFrom = (f1 + f2) / 2.0;

  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    findParameterByID({ C15::PID::Split_Split_Point, vg })->setCPFromHwui(transaction, meanFadeFrom);
  }

  findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::II })->stepCPFromHwui(transaction, 1, {});
}

void EditBuffer::loadSinglePresetIntoSplitPart(UNDO::Transaction *transaction, const Preset *preset,
                                               VoiceGroup loadInto)
{
  {
    auto toFxParam = findParameterByID({ C15::PID::Out_Mix_To_FX, loadInto });
    auto splitPointI = findParameterByID({C15::PID::Split_Split_Point, VoiceGroup::I});
    auto splitPointII = findParameterByID({C15::PID::Split_Split_Point, VoiceGroup::II});

    ScopedLock locks(transaction);
    locks.addLock(toFxParam);
    locks.addLock(splitPointI);
    locks.addLock(splitPointII);

    super::copyFrom(transaction, preset, VoiceGroup::I, loadInto);
  }

  copySinglePresetMasterToPartMaster(transaction, preset, loadInto);
  setVoiceGroupName(transaction, preset->getName(), loadInto);
  initCrossFB(transaction);
  initFadeFrom(transaction, loadInto);
}

void EditBuffer::loadSinglePresetIntoLayerPart(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup loadTo)
{
  auto toFxParam = findParameterByID({ C15::PID::Out_Mix_To_FX, loadTo });
  auto fadeFromParams = { findParameterByID({ C15::PID::Voice_Grp_Fade_From, loadTo }),
                          findParameterByID({ C15::PID::Voice_Grp_Fade_Range, loadTo }) };

  {
    ScopedLock locks(transaction);
    locks.addLock(toFxParam);
    locks.addGroupLock({ "Unison", VoiceGroup::I });
    locks.addGroupLock({ "Unison", VoiceGroup::II });
    locks.addGroupLock({ "Mono", VoiceGroup::I });
    locks.addGroupLock({ "Mono", VoiceGroup::II });

    for(auto p : getCrossFBParameters(loadTo))
      locks.addLock(p);

    for(auto p : fadeFromParams)
      locks.addLock(p);

    super::copyFrom(transaction, preset, VoiceGroup::I, loadTo);
  }

  copySinglePresetMasterToPartMaster(transaction, preset, loadTo);

  getParameterGroupByID({ "Unison", VoiceGroup::II })->undoableLoadDefault(transaction, Defaults::FactoryDefault);
  getParameterGroupByID({ "Mono", VoiceGroup::II })->undoableLoadDefault(transaction, Defaults::FactoryDefault);

  setVoiceGroupName(transaction, preset->getName(), loadTo);
}

void EditBuffer::undoableLoadPresetPartIntoSplitSound(UNDO::Transaction *transaction, const Preset *preset,
                                                      VoiceGroup from, VoiceGroup copyTo)
{
  SendEditBufferScopeGuard scopeGuard(transaction, true);
  setVoiceGroupName(transaction, preset->getName(), copyTo);

  {
    auto toFxParam = findParameterByID({ C15::PID::Out_Mix_To_FX, copyTo });
    ScopedLock locks(transaction);

    for(auto p : getCrossFBParameters(copyTo))
      locks.addLock(p);

    for(auto vg : { VoiceGroup::I, VoiceGroup::II })
      locks.addLock(findParameterByID({ C15::PID::Split_Split_Point, vg }));

    if(!preset->isDual())
      locks.addLock(toFxParam);

    super::copyFrom(transaction, preset, from, copyTo);
  }

  if(preset->isDual())
    copySumOfMasterGroupToVoiceGroupMasterGroup(transaction, preset, from, copyTo);
  else
    copySinglePresetMasterToPartMaster(transaction, preset, copyTo);

  initCrossFB(transaction);

  initFadeParameters(transaction, copyTo);
  if(preset->getType() == SoundType::Layer)
  {
    initFadeParameters(transaction, invert(copyTo));

    const auto unisonTo = GroupId { "Unison", copyTo };
    const auto unisonI = GroupId { "Unison", VoiceGroup::I };
    const auto monoTo = GroupId { "Mono", copyTo };
    const auto monoI = GroupId { "Mono", VoiceGroup::I };

    getParameterGroupByID(unisonTo)->copyFrom(transaction, preset->findParameterGroup(unisonI));
    getParameterGroupByID(monoTo)->copyFrom(transaction, preset->findParameterGroup(monoI));
  }

  if(preset->isDual())
    setVoiceGroupName(transaction, preset->getVoiceGroupName(from), copyTo);
  else
    setVoiceGroupName(transaction, preset->getName(), copyTo);
}

void EditBuffer::undoableLoadPresetPartIntoLayerSound(UNDO::Transaction *transaction, const Preset *preset,
                                                      VoiceGroup copyFrom, VoiceGroup copyTo)
{
  SendEditBufferScopeGuard scopeGuard(transaction, true);

  setVoiceGroupName(transaction, preset->getName(), copyTo);

  {
    ScopedLock locks(transaction);
    locks.addLock({ C15::PID::Voice_Grp_Fade_From, copyTo });
    locks.addLock({ C15::PID::Voice_Grp_Fade_Range, copyTo });

    if(preset->getType() == SoundType::Split)
    {
      locks.addLocks(getCrossFBParameters(copyTo));
    }

    if(preset->isDual())
    {
      locks.addGroupLock({ "Mono", VoiceGroup::I });
      locks.addGroupLock({ "Mono", VoiceGroup::II });
      locks.addGroupLock({ "Unison", VoiceGroup::I });
      locks.addGroupLock({ "Unison", VoiceGroup::II });
    }

    super::copyFrom(transaction, preset, copyFrom, copyTo);

    if(preset->isDual())
      copySumOfMasterGroupToVoiceGroupMasterGroup(transaction, preset, copyFrom, copyTo);
    else
      copySinglePresetMasterToPartMaster(transaction, preset, copyTo);

    initFadeParameters(transaction, copyTo);
  }

  getParameterGroupByID({ "Unison", VoiceGroup::II })->undoableLoadDefault(transaction, Defaults::FactoryDefault);
  getParameterGroupByID({ "Mono", VoiceGroup::II })->undoableLoadDefault(transaction, Defaults::FactoryDefault);

  if(preset->isDual())
    setVoiceGroupName(transaction, preset->getVoiceGroupName(copyFrom), copyTo);
  else
    setVoiceGroupName(transaction, preset->getName(), copyTo);
}

void EditBuffer::undoableLoadPresetPartIntoSingleSound(UNDO::Transaction *transaction, const Preset *preset,
                                                       VoiceGroup copyFrom, VoiceGroup copyTo)
{
  if(!preset->isDual())
    copyFrom = VoiceGroup::I;

  SendEditBufferScopeGuard scopeGuard(transaction, true);

  setVoiceGroupName(transaction, preset->getName(), copyTo);
  super::copyFrom(transaction, preset, copyFrom, copyTo);

  if(preset->isDual())
    copySumOfMasterGroupToVoiceGroupMasterGroup(transaction, preset, copyFrom, copyTo);
  else
    copySinglePresetMasterToPartMaster(transaction, preset, copyTo);

  if(preset->isDual())
    setVoiceGroupName(transaction, preset->getVoiceGroupName(copyFrom), copyTo);
  else
    setVoiceGroupName(transaction, preset->getName(), copyTo);

  initFadeParameters(transaction, copyTo);
}

void EditBuffer::undoableLoadSelectedToPart(VoiceGroup from, VoiceGroup to)
{
  if(auto selectedPreset = getParent()->getSelectedPreset())
  {
    EditBufferUseCases useCase(this);
    useCase.undoableLoadToPart(selectedPreset, from, to);
  }
}

void EditBuffer::cleanupParameterSelection(UNDO::Transaction *transaction, SoundType oldType, SoundType newType)
{
  auto scope = std::make_unique<GenericScopeGuard>([&] { lockParameterFocusChanges(); },
                                                   [&] { unlockParameterFocusChanges(); });

  using ParameterNumberMap = std::unordered_map<int, int>;
  using From = SoundType;
  using To = SoundType;
  using Conversion = std::pair<From, To>;
  using ConversionMap = std::map<Conversion, ParameterNumberMap>;

  static ConversionMap conversions { { { From::Layer, To::Split },
                                       { { C15::PID::FB_Mix_Osc, C15::PID::FB_Mix_Osc },
                                         { C15::PID::FB_Mix_Osc_Src, C15::PID::FB_Mix_Osc },
                                         { C15::PID::FB_Mix_Comb_Src, C15::PID::FB_Mix_Comb },
                                         { C15::PID::FB_Mix_SVF_Src, C15::PID::FB_Mix_SVF },
                                         { C15::PID::FB_Mix_FX_Src, C15::PID::FB_Mix_FX },
                                         { C15::PID::Voice_Grp_Fade_From, C15::PID::Voice_Grp_Volume },
                                         { C15::PID::Voice_Grp_Fade_Range, C15::PID::Voice_Grp_Volume } } },
                                     { { From::Layer, To::Single },
                                       { { C15::PID::FB_Mix_Osc, C15::PID::FB_Mix_Osc },
                                         { C15::PID::FB_Mix_Osc_Src, C15::PID::FB_Mix_Osc },
                                         { C15::PID::FB_Mix_Comb_Src, C15::PID::FB_Mix_Comb },
                                         { C15::PID::FB_Mix_SVF_Src, C15::PID::FB_Mix_SVF },
                                         { C15::PID::FB_Mix_FX_Src, C15::PID::FB_Mix_FX },
                                         { C15::PID::Out_Mix_To_FX, C15::PID::Out_Mix_Lvl },
                                         { C15::PID::Voice_Grp_Volume, C15::PID::Master_Volume },
                                         { C15::PID::Voice_Grp_Tune, C15::PID::Master_Tune },
                                         { C15::PID::Voice_Grp_Fade_From, C15::PID::Master_Volume },
                                         { C15::PID::Voice_Grp_Fade_Range, C15::PID::Master_Volume } } },
                                     { { From::Split, To::Layer },
                                       { { C15::PID::Split_Split_Point, C15::PID::Voice_Grp_Volume } } },
                                     { { From::Split, To::Single },
                                       { { C15::PID::Split_Split_Point, C15::PID::Master_Volume },
                                         { C15::PID::Voice_Grp_Tune, C15::PID::Master_Tune },
                                         { C15::PID::Voice_Grp_Volume, C15::PID::Master_Volume },
                                         { C15::PID::Out_Mix_To_FX, C15::PID::Out_Mix_Lvl } } } };

  if(Application::exists())
  {
    auto hwui = Application::get().getHWUI();

    auto currentVg = hwui->getCurrentVoiceGroup();

    auto itMap = conversions.find({ oldType, newType });
    if(itMap != conversions.end())
    {
      const auto &conv = itMap->second;
      auto id = getSelectedParameterNumber();
      auto itConv = conv.find(id);
      if(itConv != conv.end())
      {
        auto vg = ParameterId::isGlobal(itConv->second) ? VoiceGroup::Global : currentVg;

        if(newType == SoundType::Single && vg == VoiceGroup::II)
          vg = VoiceGroup::I;

        undoableSelectParameter(transaction, { itConv->second, vg }, false);
        hwui->setCurrentVoiceGroup(vg);
      }
    }

    if(newType == SoundType::Single && currentVg == VoiceGroup::II)
    {
      auto selNum = getSelectedParameterNumber();
      if(!ParameterId::isGlobal(selNum))
        undoableSelectParameter(transaction, { selNum, VoiceGroup::I }, false);

      hwui->setCurrentVoiceGroup(VoiceGroup::I);
    }
  }
}

int EditBuffer::getSelectedParameterNumber() const
{
  return m_lastSelectedParameter.getNumber();
}

std::string EditBuffer::getPresetOriginDescription() const
{
  if(m_lastLoadedPreset == Uuid::init())
    return "Init";

  if(m_lastLoadedPreset == Uuid::converted())
    return "Converted";

  return m_presetOriginDescription;
}

LoadedPresetLog *EditBuffer::getLoadedPresetLog() const
{
  return m_loadedPresetLog.get();
}

void EditBuffer::fakeParameterSelectionSignal(VoiceGroup oldGroup, VoiceGroup group)
{
  m_signalSelectedParameter.send(getSelected(oldGroup), getSelected(group));
}

bool EditBuffer::isPartLabelChanged(VoiceGroup group) const
{
  if(auto preset = getOrigin())
  {
    return preset->getVoiceGroupName(group) != getVoiceGroupName(group);
  }
  return false;
}

void EditBuffer::cleanupSplitPointIfOldPreset(UNDO::Transaction *transaction, const Preset *p)
{
  //if global split point is present we have loaded an old preset -> split II should be incremented by one
  if(p->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::Global }, false))
  {
    findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::II })->stepCPFromHwui(transaction, 1, {});
  }
}

void EditBuffer::setSyncSplitSettingAccordingToLoadedPreset(UNDO::Transaction *transaction)
{
  if(getType() == SoundType::Split)
  {
    const auto sI = findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I });

    if(sI->hasOverlap())
    {
      SyncSplitSettingUseCases::get().disableSyncSetting(transaction);
    }
    else
    {
      SyncSplitSettingUseCases::get().enableSyncSetting(transaction);
    }
  }
}

sigc::connection EditBuffer::onParameterReselected(const sigc::slot<void, Parameter *> &s)
{
  return m_signalReselectParameter.connect(s);
}

void EditBuffer::undoableSetTypeFromConvert(UNDO::Transaction *transaction, SoundType type)
{
  if(m_type != type)
  {
    auto swap = UNDO::createSwapData(type);

    cleanupParameterSelection(transaction, m_type, type);

    transaction->addSimpleCommand([=](auto state) {
      swap->swapWith(m_type);
      initUnisonVoicesScaling(m_type);
      m_signalTypeChanged.send(m_type);
      m_signalConversionHappened.send(m_type);
      onChange();
    });
  }
}
