#include "PhysicalControlBehaviorLabel.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/PhysicalControlParameter.h>
#include <proxies/hwui/FrameBuffer.h>
#include <sigc++/sigc++.h>
#include <proxies/hwui/HWUI.h>

PhysicalControlBehaviorLabel::PhysicalControlBehaviorLabel(const Rect &pos)
    : Label(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &PhysicalControlBehaviorLabel::setParameter)),
      getHWUI()->getCurrentVoiceGroup());
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

void PhysicalControlBehaviorLabel::setBackgroundColor(FrameBuffer &fb) const
{
  if(isVisible())
    Control::setBackgroundColor(fb);
  else
    fb.setColor(FrameBufferColors::Transparent);
}
