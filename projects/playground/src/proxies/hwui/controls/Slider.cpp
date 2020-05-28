#include "Slider.h"
#include "parameters/Parameter.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>
#include <sigc++/adaptors/hide.h>

Slider::Slider(Parameter *param, const Rect &rect)
    : super(rect)
    , m_value(0)
    , m_bipolar(false)
{
  Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::hide(sigc::mem_fun(this, &Slider::onSoundTypeChanged)), false);
  setParameter(param);
}

Slider::Slider(const Rect &rect)
    : Slider(nullptr, rect)
{
}

void Slider::setParameter(Parameter *param)
{
  if(m_param != param)
  {
    m_param = param;

    m_paramValueConnection.disconnect();

    if(param)
      m_paramValueConnection = param->onParameterChanged(sigc::mem_fun(this, &Slider::onParamValueChanged));
    else
      setValue(0, false);

    auto visible = ParameterSelectLayout2::isParameterAvailableInSoundType(m_param);
    setVisible(visible);
    setDirty();
  }
}

Parameter *Slider::getParameter() const
{
  return m_param;
}

void Slider::onParamValueChanged(const Parameter *param)
{
  setValue(param->getControlPositionValue(), param->isBiPolar());
}

void Slider::onSoundTypeChanged()
{
  setVisible(ParameterLayout2::isParameterAvailableInSoundType(m_param));
}

void Slider::setValue(tDisplayValue v, bool bipolar)
{
  if(m_value != v || m_bipolar != bipolar)
  {
    m_value = v;
    m_bipolar = bipolar;
    setDirty();
  }
}

tDisplayValue Slider::getValue() const
{
  return m_value;
}

void Slider::setSliderColor(FrameBuffer &fb)
{
  if(isHighlight())
    fb.setColor(FrameBufferColors::C255);
  else
    fb.setColor(FrameBufferColors::C103);
}

bool Slider::hasBorder() const
{
  return true;
}

bool Slider::isBiPolar() const
{
  return m_bipolar;
}

void Slider::drawBackground(FrameBuffer &fb)
{
  if(isVisible())
    Control::drawBackground(fb);
}
