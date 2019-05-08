#include "ModulationRoutingParameter.h"
#include "PhysicalControlParameter.h"
#include "MacroControlParameter.h"
#include "RibbonParameter.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <groups/MacroControlMappingGroup.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulationRouterParameterLayouts.h>
#include <cmath>

ModulationRoutingParameter::ModulationRoutingParameter(ParameterGroup *group, uint16_t id, tSrcParameterPtr srcParam,
                                                       tMCParameterPtr tgtParam, const ScaleConverter *scaling)
    : super(group, id, scaling, 0, 100, 1000)
    , m_tgtParameter(tgtParam)
    , m_srcParameter(srcParam)
{
  srcParam->registerTarget(this);
}

ModulationRoutingParameter::~ModulationRoutingParameter()
{
}

void ModulationRoutingParameter::onValueChanged(Initiator initiator, tControlPositionValue oldValue,
                                                tControlPositionValue newValue)
{
  Parameter::onValueChanged(initiator, oldValue, newValue);

  if(oldValue == 0.0 && newValue != 0)
  {
    if(auto p = dynamic_cast<RibbonParameter *>(getSourceParameter()))
    {
      if(p->getRibbonReturnMode() == RibbonParameter::STAY)
      {
        MacroControlMappingGroup *parent = dynamic_cast<MacroControlMappingGroup *>(getParentGroup());
        auto otherRouters = parent->getModulationRoutingParametersFor(getSourceParameter());

        for(auto router : otherRouters)
        {
          if(router != this)
          {
            router->onExclusiveRoutingLost();
          }
        }

        p->boundToMacroControl(getTargetParameter()->getControlPositionValue());
      }
    }
  }
}

void ModulationRoutingParameter::onExclusiveRoutingLost()
{
  getValue().setRawValue(Initiator::INDIRECT, 0);
  onChange();
  invalidate();
}

void ModulationRoutingParameter::applyLpcPhysicalControl(tControlPositionValue diff)
{
  auto amount = getControlPositionValue();

  if(std::abs(amount) > numeric_limits<tDisplayValue>::epsilon())
  {
    tControlPositionValue result = diff * amount;

    if(std::abs(result) > numeric_limits<tDisplayValue>::epsilon())
      m_tgtParameter->applyLpcPhysicalControl(result);
  }
}

void ModulationRoutingParameter::applyAbsoluteLpcPhysicalControl(tControlPositionValue v)
{
  auto amount = getControlPositionValue();

  if(std::abs(amount) > numeric_limits<tDisplayValue>::epsilon())
  {
    m_tgtParameter->applyAbsoluteLpcPhysicalControl(v);
  }
}

ModulationRoutingParameter::tMCParameterPtr ModulationRoutingParameter::getTargetParameter() const
{
  return m_tgtParameter;
}

ModulationRoutingParameter::tSrcParameterPtr ModulationRoutingParameter::getSourceParameter() const
{
  return m_srcParameter;
}

bool ModulationRoutingParameter::routes(const PhysicalControlParameter *p) const
{
  return m_srcParameter == p;
}

Glib::ustring ModulationRoutingParameter::getDisplayString() const
{
  if(getSourceParameter()->getReturnMode() == PhysicalControlParameter::ReturnMode::None)
  {
    if(getValue().getDisplayValue() != 0.0)
    {
      return "On";
    }
    else
    {
      return "Off";
    }
  }

  return super::getDisplayString();
}

tControlPositionValue ModulationRoutingParameter::getControlPositionValue() const
{
  if(getSourceParameter()->getReturnMode() == PhysicalControlParameter::ReturnMode::None)
  {
    if(getValue().getDisplayValue() != 0.0)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }

  return super::getControlPositionValue();
}

DFBLayout *ModulationRoutingParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new ModulationRouterParameterEditLayout2();

    case UIMode::Select:
    default:
      return new ModulationRouterParameterSelectLayout2();
  }

  g_return_val_if_reached(nullptr);
}

void ModulationRoutingParameter::undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator,
                                                   double amount)
{
}
