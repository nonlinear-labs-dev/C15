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

static int lastSelectedMacroControl = MacroControlsGroup::modSrcToParamID(MacroControls::MC1);

MacroControlParameter::MacroControlParameter(ParameterGroup *group, uint16_t id)
    : Parameter(group, id, ScaleConverter::get<MacroControlScaleConverter>(), 0.5, 100, 1000)
    , m_UiSelectedHardwareSourceParameterID(HardwareSourcesGroup::getPedal1ParameterID())
    , mcviewThrottler{ Expiration::Duration(5) }
    , m_lastMCViewUuid{ "NONE" }
{
}

MacroControlParameter::~MacroControlParameter() = default;

void MacroControlParameter::writeDocProperties(Writer &writer, tUpdateID knownRevision) const
{
  Parameter::writeDocProperties(writer, knownRevision);
  writer.writeTextElement("givenName", m_givenName);
  writer.writeTextElement("long-name", getLongName());
  writer.writeTextElement("short-name", getShortName());
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

void MacroControlParameter::applyLpcPhysicalControl(tControlPositionValue diff)
{
  getValue().changeRawValue(Initiator::EXPLICIT_LPC, diff);
}

void MacroControlParameter::applyAbsoluteLpcPhysicalControl(tControlPositionValue v)
{
  getValue().setRawValue(Initiator::EXPLICIT_LPC, v);
}

void MacroControlParameter::onValueChanged(Initiator initiator, tControlPositionValue oldValue,
                                           tControlPositionValue newValue)
{
  super::onValueChanged(initiator, oldValue, newValue);

  if(initiator != Initiator::INDIRECT)
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
      target->applyLpcMacroControl(newValue - oldValue);

  super::onValueFineQuantizedChanged(initiator, oldValue, newValue);
}

void MacroControlParameter::updateMCViewsFromMCChange(const Initiator &initiator)
{
  mcviewThrottler.doTask([=]() { propagateMCChangeToMCViews(initiator); });
}

void MacroControlParameter::propagateMCChangeToMCViews(const Initiator &initiatior)
{
  const auto idString = to_string(getID());
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
  auto groups = dynamic_cast<ParameterGroupSet *>(getParentGroup()->getParent());
  auto mcm = dynamic_cast<MacroControlMappingGroup *>(groups->getParameterGroupByID("MCM"));
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

void MacroControlParameter::setUiSelectedHardwareSource(int pos)
{
  if(m_UiSelectedHardwareSourceParameterID != pos)
  {
    auto *grandPa = dynamic_cast<ParameterGroupSet *>(getParent()->getParent());

    if(auto old = grandPa->findParameterByID(m_UiSelectedHardwareSourceParameterID))
      old->onUnselected();

    m_UiSelectedHardwareSourceParameterID = pos;
    invalidate();
  }
}

void MacroControlParameter::toggleUiSelectedHardwareSource(int inc)
{
  int id = getUiSelectedHardwareSource();

  auto grandPa = dynamic_cast<ParameterGroupSet *>(getParent()->getParent());
  auto controlSources = dynamic_cast<HardwareSourcesGroup *>(grandPa->getParameterGroupByID("CS"));
  auto availableSources = controlSources->getPhysicalControlParameters();
  setUiSelectedHardwareSource(getIdOfAdvancedParameter(availableSources, id, inc));
}

int MacroControlParameter::getUiSelectedHardwareSource() const
{
  return m_UiSelectedHardwareSourceParameterID;
}

void MacroControlParameter::undoableSetGivenName(const Glib::ustring &newName)
{
  if(m_givenName != newName)
  {
    auto scope = getUndoScope().startTransaction("Rename MacroControl '%0'", getLongName());
    undoableSetGivenName(scope->getTransaction(), newName);
  }
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

void MacroControlParameter::undoableSetInfo(const Glib::ustring &info)
{
  if(m_info != info)
  {
    auto scope = getUndoScope().startTransaction("Set MacroControl Info '%0'", getLongName());
    undoableSetInfo(scope->getTransaction(), info);
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

void MacroControlParameter::undoableResetConnectionsToTargets()
{
  auto scope = getUndoScope().startTransaction("Reset MacroControls connections");

  while(!m_targets.empty())
  {
    auto p = *m_targets.begin();
    p->undoableSelectModSource(scope->getTransaction(), MacroControls::NONE);
    p->undoableSetModAmount(scope->getTransaction(), 0);
  }
}

void MacroControlParameter::loadDefault(UNDO::Transaction *transaction)
{
  super::loadDefault(transaction);
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

bool MacroControlParameter::isSourceOfTargetIn(const std::list<gint32> &ids) const
{
  for(auto t : m_targets)
    for(int id : ids)
      if(t->getID() == id)
        return true;

  return false;
}

bool MacroControlParameter::isSourceOf(gint32 id) const
{
  for(auto t : m_targets)
    if(t->getID() == id)
      return true;

  return false;
}

DFBLayout *MacroControlParameter::createLayout(FocusAndMode focusAndMode) const
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

  g_return_val_if_reached(nullptr);
}

void MacroControlParameter::onSelected()
{
  super::onSelected();
  lastSelectedMacroControl = getID();
}

void MacroControlParameter::onUnselected()
{
  auto grandPa = dynamic_cast<ParameterGroupSet *>(getParent()->getParent());
  auto controlSources = dynamic_cast<HardwareSourcesGroup *>(grandPa->getParameterGroupByID("CS"));

  for(auto source : controlSources->getPhysicalControlParameters())
    source->onUnselected();
}

int MacroControlParameter::getLastSelectedMacroControl()
{
  return lastSelectedMacroControl;
}

void MacroControlParameter::undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount)
{
}

void MacroControlParameter::setCPFromMCView(UNDO::Transaction *transaction, const tControlPositionValue &cpValue)
{
  setCpValue(transaction, Initiator::EXPLICIT_MCVIEW, cpValue, true);
}
