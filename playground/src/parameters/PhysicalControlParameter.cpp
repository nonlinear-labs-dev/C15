#include "PhysicalControlParameter.h"
#include "ModulationRoutingParameter.h"
#include "MacroControlParameter.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/InfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/UnmodulatebaleParameterLayouts.h>
#include <proxies/hwui/HWUI.h>
#include <device-settings/DebugLevel.h>
#include <groups/MacroControlMappingGroup.h>
#include <groups/HardwareSourcesGroup.h>
#include <presets/ParameterGroupSet.h>
#include <limits>
#include <math.h>
#include "ParameterAlgorithm.h"
#include <http/UpdateDocumentMaster.h>
#include <Application.h>
#include <proxies/hwui/panel-unit/PanelUnitParameterEditMode.h>

PhysicalControlParameter::PhysicalControlParameter(ParameterGroup *group, uint16_t id, const ScaleConverter *scaling,
                                                   tDisplayValue def, int coarseDenominator, int fineDenominator)
    : super(group, id, scaling, def, coarseDenominator, fineDenominator)
{
}

PhysicalControlParameter::~PhysicalControlParameter()
{
}

void PhysicalControlParameter::onChangeFromLpc(tControlPositionValue newVal)
{
  getValue().setRawValue(Initiator::EXPLICIT_LPC, getValue().clip(newVal));
}

void PhysicalControlParameter::setCPFromHwui(UNDO::Scope::tTransactionPtr transaction,
                                             const tControlPositionValue &cpValue)
{
  m_changingFromHWUI = true;
  Parameter::setCPFromHwui(transaction, cpValue);
  m_changingFromHWUI = false;
  m_lastChangedFromHWUI = true;
}

void PhysicalControlParameter::setCPFromWebUI(UNDO::Scope::tTransactionPtr transaction,
                                              const tControlPositionValue &cpValue)
{
  Parameter::setCPFromWebUI(transaction, cpValue);
}

void PhysicalControlParameter::onValueChanged(Initiator initiator, tControlPositionValue oldValue,
                                              tControlPositionValue newValue)
{
  if(!m_changingFromHWUI)
  {
    m_lastChangedFromHWUI = false;
  }

  if(initiator != Initiator::INDIRECT)
  {
    if(getReturnMode() != ReturnMode::None)
    {
      for(ModulationRoutingParameter *target : m_targets)
        target->applyLpcPhysicalControl(newValue - oldValue);
    }
    else
    {
      for(ModulationRoutingParameter *target : m_targets)
        target->applyAbsoluteLpcPhysicalControl(newValue);
    }
  }

  super::onValueChanged(initiator, oldValue, newValue);
}

Glib::ustring PhysicalControlParameter::getDisplayString() const
{
  if(m_lastChangedFromHWUI && getReturnMode() != ReturnMode::None && getControlPositionValue() != 0.0)
    return Glib::ustring("! ") + Parameter::getDisplayString();

  return Parameter::getDisplayString();
}

void PhysicalControlParameter::loadFromPreset(UNDO::Scope::tTransactionPtr transaction,
                                              const tControlPositionValue &value)
{
}

void PhysicalControlParameter::registerTarget(ModulationRoutingParameter *target)
{
  m_targets.append(target);
}

size_t PhysicalControlParameter::getHash() const
{
  size_t hash = 0;
  hash_combine(hash, getLongName());
  return hash;
}

Glib::ustring PhysicalControlParameter::generateName() const
{
  auto it = max_element(m_targets.begin(), m_targets.end(),
                        [](const ModulationRoutingParameter *a, const ModulationRoutingParameter *b) {
                          auto fa = fabs(a->getControlPositionValue());
                          auto fb = fabs(b->getControlPositionValue());

                          if(fa == fb)
                            return a->getID() < b->getID();

                          return fa < fb;
                        });

  if(it == m_targets.end())
    return "Not assigned";

  ModulationRoutingParameter *target = *it;

  if(target->getControlPositionValue() == 0)
    return "Not assigned";

  return target->getTargetParameter()->getLongName();
}

DFBLayout *PhysicalControlParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    default:
      return super::createLayout(focusAndMode);
  }

  g_return_val_if_reached(nullptr);
}

void PhysicalControlParameter::onSelected()
{
  super::onSelected();

  auto grandPa = dynamic_cast<const ParameterGroupSet *>(getParent()->getParent());
  auto lastSelectedMacroControl = MacroControlParameter::getLastSelectedMacroControl();
  auto mc = dynamic_cast<MacroControlParameter *>(grandPa->findParameterByID(lastSelectedMacroControl));
  mc->setUiSelectedHardwareSource(getID());
}

void PhysicalControlParameter::onUnselected()
{
  Parameter::onUnselected();

  if(m_lastChangedFromHWUI && getReturnMode() != ReturnMode::None)
  {
    m_lastChangedFromHWUI = false;
    getValue().setRawValue(Initiator::EXPLICIT_OTHER, 0);
    sendToLpc();
    onChange();
    invalidate();
  }
}

void PhysicalControlParameter::setUiSelectedModulationRouter(int pos)
{
  auto current = getUiSelectedModulationRouter();

  if(current != pos)
  {
    ParameterGroupSet *grandPa = dynamic_cast<ParameterGroupSet *>(getParent()->getParent());

    if(auto oldRouter = grandPa->findParameterByID(current))
      oldRouter->onUnselected();

    if(auto newRouter = dynamic_cast<ModulationRoutingParameter *>(grandPa->findParameterByID(pos)))
    {
      newRouter->getTargetParameter()->onSelected();
    }

    invalidate();

    if(auto u
       = dynamic_pointer_cast<PanelUnitParameterEditMode>(Application::get().getHWUI()->getPanelUnit().getUsageMode()))
    {
      u->bruteForceUpdateLeds();
    }
  }
}

void PhysicalControlParameter::toggleUiSelectedModulationRouter(int inc)
{
  int id = getUiSelectedModulationRouter();

  ParameterGroupSet *grandPa = dynamic_cast<ParameterGroupSet *>(getParent()->getParent());
  auto mappings = dynamic_cast<MacroControlMappingGroup *>(grandPa->getParameterGroupByID("MCM"));
  auto routers = mappings->getModulationRoutingParametersFor(this);
  setUiSelectedModulationRouter(getIdOfAdvancedParameter(routers, id, inc));
}

int PhysicalControlParameter::getUiSelectedModulationRouter() const
{
  auto grandPa = dynamic_cast<const ParameterGroupSet *>(getParent()->getParent());
  auto lastSelectedMacroControl = MacroControlParameter::getLastSelectedMacroControl();
  auto mc = dynamic_cast<MacroControlParameter *>(grandPa->findParameterByID(lastSelectedMacroControl));

  auto mcm = dynamic_cast<MacroControlMappingGroup *>(grandPa->getParameterGroupByID("MCM"));

  if(auto router = mcm->getModulationRoutingParameterFor(this, mc))
    return router->getID();

  return mcm->getModulationRoutingParametersFor(this).front()->getID();
}

bool PhysicalControlParameter::hasBehavior() const
{
  return false;
}

Glib::ustring PhysicalControlParameter::getCurrentBehavior() const
{
  throw bad_function_call();
}

void PhysicalControlParameter::undoableStepBehavior(UNDO::Scope::tTransactionPtr transaction, int direction)
{
  throw bad_function_call();
}

void PhysicalControlParameter::undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator,
                                                 double amount)
{
}
