#include <utility>

#include "MacroControlParameter.h"
#include "ModulateableParameter.h"
#include "PhysicalControlParameter.h"
#include "scale-converters/MacroControlScaleConverter.h"
#include "presets/ParameterGroupSet.h"
#include "groups/HardwareSourcesGroup.h"
#include <groups/MacroControlMappingGroup.h>
#include <groups/MacroControlsGroup.h>
#include "xml/Writer.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/RenameMCLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/MacroControlParameterLayouts.h>
#include "parameters/ModulationRoutingParameter.h"
#include "ParameterAlgorithm.h"
#include "RibbonParameter.h"
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <nltools/threading/Throttler.h>
#include "http/HTTPServer.h"
#include <libundo/undo/Transaction.h>
#include <presets/PresetParameter.h>
#include <tools/StringTools.h>
#include <presets/EditBuffer.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <xml/Attribute.h>
#include <presets/recall/RecallParameter.h>

static ParameterId lastSelectedMacroControl = MacroControlsGroup::modSrcToParamId(MacroControls::MC1);

MacroControlParameter::MacroControlParameter(ParameterGroup *group, const ParameterId &id)
    : Parameter(group, id, ScaleConverter::get<MacroControlScaleConverter>(), 0.5, 100, 1000)
    , m_UiSelectedHardwareSourceParameterID(HardwareSourcesGroup::getPedal1ParameterID().getNumber(),
                                            id.getVoiceGroup())
    , m_lastMCViewUuid { "NONE" }
    , mcviewThrottler { Expiration::Duration(5) }
{
}

MacroControlParameter::~MacroControlParameter() = default;

void MacroControlParameter::writeDocProperties(Writer &writer, tUpdateID knownRevision) const
{
  Parameter::writeDocProperties(writer, knownRevision);
  writer.writeTextElement("givenName", m_givenName);
  writer.writeTextElement("long-name", getLongName());
  writer.writeTextElement("short-name", getShortName());
  if(auto og = getOriginalParameter())
  {
    writer.writeTextElement("og-info", og->getInfo());
    writer.writeTextElement("og-name", og->getGivenName());
  }
  writer.writeTextElement("info", m_info);
}

void MacroControlParameter::writeDifferences(Writer &writer, Parameter *other) const
{
  Parameter::writeDifferences(writer, other);
  auto *pOther = static_cast<MacroControlParameter *>(other);

  if(getGivenName() != pOther->getGivenName())
  {
    writer.writeTextElement("name", "", Attribute("a", getGivenName()), Attribute("b", pOther->getGivenName()));
  }

  if(getInfo() != pOther->getInfo())
  {
    writer.writeTextElement("info", "", Attribute("a", "changed"), Attribute("b", "changed"));
  }
}

void MacroControlParameter::registerTarget(ModulateableParameter *target)
{
  m_targets.insert(target);
  m_targetListChanged();
}

void MacroControlParameter::unregisterTarget(ModulateableParameter *target)
{
  m_targets.erase(target);
  m_targetListChanged();
}

void MacroControlParameter::setLastMCViewUUID(const Glib::ustring &uuid)
{
  m_lastMCViewUuid = uuid;
}

void MacroControlParameter::applyPlaycontrollerPhysicalControl(tControlPositionValue diff)
{
  getValue().changeRawValue(Initiator::EXPLICIT_PLAYCONTROLLER, diff);
}

void MacroControlParameter::applyAbsolutePlaycontrollerPhysicalControl(tControlPositionValue v)
{
  getValue().setRawValue(Initiator::EXPLICIT_PLAYCONTROLLER, v);
}

void MacroControlParameter::onValueChanged(Initiator initiator, tControlPositionValue oldValue,
                                           tControlPositionValue newValue)
{
  super::onValueChanged(initiator, oldValue, newValue);

  updateBoundRibbon();

  for(ModulateableParameter *target : m_targets)
    target->invalidate();

  updateMCViewsFromMCChange(initiator);
}

void MacroControlParameter::onValueFineQuantizedChanged(Initiator initiator, tControlPositionValue oldValue,
                                                        tControlPositionValue newValue)
{
  if(initiator != Initiator::INDIRECT)
    for(ModulateableParameter *target : m_targets)
      target->applyPlaycontrollerMacroControl(newValue - oldValue);

  super::onValueFineQuantizedChanged(initiator, oldValue, newValue);
}

void MacroControlParameter::updateMCViewsFromMCChange(const Initiator &initiator)
{
  mcviewThrottler.doTask([=]() { propagateMCChangeToMCViews(initiator); });
}

void MacroControlParameter::propagateMCChangeToMCViews(const Initiator &initiatior)
{
  const auto idString = getID().toString();
  const auto valueD = getValue().getClippedValue();
  const auto value = to_string(valueD);
  const auto uuid = initiatior == Initiator::EXPLICIT_MCVIEW ? m_lastMCViewUuid.c_str() : "NONE";

  if(valueD != lastBroadcastedControlPosition)
  {
    const auto str
        = StringTools::buildString("MCVIEW&ID=", idString, "&VAL=", value, "&UUID=", uuid, "&NAME=", getGivenName());
    Application::get().getHTTPServer()->getMCViewContentManager().sendToAllWebsockets(str);
    lastBroadcastedControlPosition = valueD;
  }
}

void MacroControlParameter::updateBoundRibbon()
{
  if(auto groups = dynamic_cast<ParameterGroupSet *>(getParentGroup()->getParent()))
  {
    auto mcm = dynamic_cast<MacroControlMappingGroup *>(groups->getParameterGroupByID({ "MCM", VoiceGroup::Global }));

    auto routers = mcm->getModulationRoutingParametersFor(this);

    for(auto router : routers)
    {
      if(auto ribbon = dynamic_cast<RibbonParameter *>(router->getSourceParameter()))
      {
        if(router->getControlPositionValue() > 0)
        {
          if(ribbon->getRibbonReturnMode() == RibbonReturnMode::STAY)
          {
            ribbon->boundToMacroControl(getControlPositionValue());
          }
        }
      }
    }
  }
}

void MacroControlParameter::setUiSelectedHardwareSource(ParameterId number)
{
  if(m_UiSelectedHardwareSourceParameterID != number)
  {
    if(auto *eb = dynamic_cast<ParameterGroupSet *>(getParent()->getParent()))
    {
      if(auto old = eb->findParameterByID(m_UiSelectedHardwareSourceParameterID))
        old->onUnselected();

      m_UiSelectedHardwareSourceParameterID = number;
      invalidate();
    }
  }
}

void MacroControlParameter::toggleUiSelectedHardwareSource(int inc)
{
  auto id = getUiSelectedHardwareSource();

  auto grandPa = dynamic_cast<ParameterGroupSet *>(getParent()->getParent());
  auto controlSources
      = dynamic_cast<HardwareSourcesGroup *>(grandPa->getParameterGroupByID({ "CS", VoiceGroup::Global }));
  auto availableSources = controlSources->getPhysicalControlParameters();
  setUiSelectedHardwareSource(getIdOfAdvancedParameter(availableSources, id, inc));
}

ParameterId MacroControlParameter::getUiSelectedHardwareSource() const
{
  return m_UiSelectedHardwareSourceParameterID;
}

void MacroControlParameter::undoableSetGivenName(UNDO::Transaction *transaction, const Glib::ustring &newName)
{
  if(m_givenName != newName)
  {
    auto swapData = UNDO::createSwapData(newName);

    DebugLevel::info("Set given name of MC to", newName);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      swapData->swapWith(m_givenName);
      invalidate();
    });
  }
}

void MacroControlParameter::undoableSetInfo(UNDO::Transaction *transaction, const Glib::ustring &info)
{
  if(m_info != info)
  {
    auto swapData = UNDO::createSwapData(info);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      swapData->swapWith(m_info);
      invalidate();
    });
  }
}

bool MacroControlParameter::isChangedFromLoaded() const
{
  if(auto og = getOriginalParameter())
  {
    auto nameSame = m_givenName == og->getGivenName();
    auto infoSame = m_info == og->getInfo();
    return !nameSame || !infoSame || Parameter::isChangedFromLoaded();
  }
  return Parameter::isChangedFromLoaded();
}

void MacroControlParameter::undoableResetConnectionsToTargets()
{
}

void MacroControlParameter::loadDefault(UNDO::Transaction *transaction, Defaults mode)
{
  super::loadDefault(transaction, mode);
  undoableSetGivenName(transaction, "");
  undoableSetInfo(transaction, "");
}

void MacroControlParameter::copyFrom(UNDO::Transaction *transaction, const PresetParameter *other)
{
  if(!isLocked())
  {
    super::copyFrom(transaction, other);
    undoableSetGivenName(transaction, other->getGivenName());
    undoableSetInfo(transaction, other->getInfo());

    transaction->addPostfixCommand([=](UNDO::Command::State) mutable { updateBoundRibbon(); });
  }
}

void MacroControlParameter::copyTo(UNDO::Transaction *transaction, PresetParameter *other) const
{
  super::copyTo(transaction, other);
  other->setField(transaction, PresetParameter::Fields::GivenName, getGivenName());
  other->setField(transaction, PresetParameter::Fields::Info, getInfo());
}

const Glib::ustring &MacroControlParameter::getGivenName() const
{
  return m_givenName;
}

const Glib::ustring &MacroControlParameter::getInfo() const
{
  return m_info;
}

Glib::ustring MacroControlParameter::getLongName() const
{
  Glib::ustring b = super::getLongName();

  if(!m_givenName.empty())
    return b + " " + m_givenName;

  return b;
}

size_t MacroControlParameter::getHash() const
{
  size_t hash = super::getHash();
  hash_combine(hash, getLongName());
  hash_combine(hash, getInfo());
  return hash;
}

const MacroControlParameter::tTargets &MacroControlParameter::getTargets() const
{
  return m_targets;
}

sigc::connection MacroControlParameter::onTargetListChanged(sigc::slot<void> cb)
{
  return m_targetListChanged.connect(cb);
}

bool MacroControlParameter::isSourceOfTargetIn(const std::list<ParameterId> &ids) const
{
  for(const auto t : m_targets)
    for(const auto &id : ids)
      if(t->getID() == id)
        return true;

  return false;
}

bool MacroControlParameter::isSourceOf(ParameterId id) const
{
  for(auto t : m_targets)
    if(t->getID() == id)
      return true;

  return false;
}

Layout *MacroControlParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new MacroControlParameterEditLayout2();

    case UIMode::Select:
    default:
      return new MacroControlParameterSelectLayout2();
  }
}

void MacroControlParameter::onSelected()
{
  super::onSelected();
  lastSelectedMacroControl = getID();
}

void MacroControlParameter::onUnselected()
{
  auto grandPa = dynamic_cast<ParameterGroupSet *>(getParent()->getParent());
  auto controlSources
      = dynamic_cast<HardwareSourcesGroup *>(grandPa->getParameterGroupByID({ "CS", VoiceGroup::Global }));

  for(auto source : controlSources->getPhysicalControlParameters())
    source->onUnselected();
}

ParameterId MacroControlParameter::getLastSelectedMacroControl()
{
  return lastSelectedMacroControl;
}

ParameterId MacroControlParameter::getMCSmoothingParameterForMC(const ParameterId &mcId)
{
  switch(mcId.getNumber())
  {
    case 243:
      return { 324, VoiceGroup::Global };
    case 244:
      return { 325, VoiceGroup::Global };
    case 245:
      return { 326, VoiceGroup::Global };
    case 246:
      return { 327, VoiceGroup::Global };
    case 369:
      return { 370, VoiceGroup::Global };
    case 371:
      return { 372, VoiceGroup::Global };
    default:
      return ParameterId::invalid();
  }
}

ParameterId MacroControlParameter::getSmoothingParameter()
{
  return MacroControlParameter::getMCSmoothingParameterForMC(getID());
}

void MacroControlParameter::undoableRandomize(UNDO::Transaction *, Initiator, double)
{
}

void MacroControlParameter::setCPFromMCView(UNDO::Transaction *transaction, const tControlPositionValue &cpValue)
{
  setCpValue(transaction, Initiator::EXPLICIT_MCVIEW, cpValue, true);
}

void MacroControlParameter::sendParameterMessage() const
{
  Application::get().getAudioEngineProxy()->createAndSendParameterMessage<MacroControlParameter>(this);
}
