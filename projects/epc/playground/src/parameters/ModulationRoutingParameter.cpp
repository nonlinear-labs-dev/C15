#include "ModulationRoutingParameter.h"
#include "PhysicalControlParameter.h"
#include "MacroControlParameter.h"
#include "RibbonParameter.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <groups/MacroControlMappingGroup.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulationRouterParameterLayouts.h>
#include <cmath>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <Application.h>

ModulationRoutingParameter::ModulationRoutingParameter(ParameterGroup *group, ParameterId id, tSrcParameterPtr srcParam,
                                                       tMCParameterPtr tgtParam, const ScaleConverter *scaling)
    : super(group, id, scaling, 0, 100, 1000)
    , m_tgtParameter(tgtParam)
    , m_srcParameter(srcParam)
{
  srcParam->registerTarget(this);
}

ModulationRoutingParameter::~ModulationRoutingParameter() = default;

void ModulationRoutingParameter::onValueChanged(Initiator initiator, tControlPositionValue oldValue,
                                                tControlPositionValue newValue)
{
  Parameter::onValueChanged(initiator, oldValue, newValue);

  if(oldValue == 0.0 && newValue != 0)
  {
    if(auto p = dynamic_cast<RibbonParameter *>(getSourceParameter()))
    {
      if(p->getRibbonReturnMode() == RibbonReturnMode::STAY)
      {
        auto *parent = dynamic_cast<MacroControlMappingGroup *>(getParentGroup());
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
  sendToPlaycontroller();
}

void ModulationRoutingParameter::applyPlaycontrollerPhysicalControl(tControlPositionValue diff)
{
  auto amount = getControlPositionValue();

  if(std::abs(amount) > std::numeric_limits<tDisplayValue>::epsilon())
  {
    tControlPositionValue result = diff * amount;

    if(std::abs(result) > std::numeric_limits<tDisplayValue>::epsilon())
      m_tgtParameter->applyPlaycontrollerPhysicalControl(result);
  }
}

void ModulationRoutingParameter::applyAbsolutePlaycontrollerPhysicalControl(tControlPositionValue v)
{
  auto amount = getControlPositionValue();

  if(std::abs(amount) > std::numeric_limits<tDisplayValue>::epsilon())
  {
    m_tgtParameter->applyAbsolutePlaycontrollerPhysicalControl(v);
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
  if(getSourceParameter()->getReturnMode() == ReturnMode::None)
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
  if(getSourceParameter()->getReturnMode() == ReturnMode::None)
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

Layout *ModulationRoutingParameter::createLayout(FocusAndMode focusAndMode) const
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

void ModulationRoutingParameter::undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount)
{
}

void ModulationRoutingParameter::sendParameterMessage() const
{
  if(Application::exists())
  {
    Application::get().getAudioEngineProxy()->createAndSendParameterMessage<ModulationRoutingParameter>(this);
  }
}
