#include "PhysicalControlBehaviorLabel.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/PhysicalControlParameter.h>

PhysicalControlBehaviorLabel::PhysicalControlBehaviorLabel(const Rect &pos)
    : Label(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &PhysicalControlBehaviorLabel::setParameter)));
}

void PhysicalControlBehaviorLabel::setParameter(Parameter *param)
{
  if(param)
  {
    m_paramConnection.disconnect();
    m_paramConnection
        = param->onParameterChanged(sigc::mem_fun(this, &PhysicalControlBehaviorLabel::onParamValueChanged));
  }
}

void PhysicalControlBehaviorLabel::onParamValueChanged(const Parameter *param)
{
  if(auto p = dynamic_cast<const PhysicalControlParameter *>(param))
  {
    if(p->hasBehavior())
    {
      setText(p->getCurrentBehavior());
    }
  }
}
