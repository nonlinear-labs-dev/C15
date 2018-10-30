#include "SelectedParameterModAmount.h"
#include "Application.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "groups/MacroControlsGroup.h"

SelectedParameterModAmount::SelectedParameterModAmount(const Rect &rect)
    : super(rect)
    , m_from(0)
    , m_to(0)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &SelectedParameterModAmount::setParameter)));

  Application::get().getPresetManager()->getEditBuffer()->onPresetChanged(
      (sigc::mem_fun(this, &SelectedParameterModAmount::onPresetChanged)));
}

SelectedParameterModAmount::~SelectedParameterModAmount()
{
}

void SelectedParameterModAmount::onPresetChanged()
{
  if(auto eb = Application::get().getPresetManager()->getEditBuffer()) {
    if(auto sel = eb->getSelected()) {
      setParameter(sel);
    }
  }
}

void SelectedParameterModAmount::setParameter(Parameter *param)
{
  if(param)
  {
    m_paramValueConnection.disconnect();
    m_paramValueConnection
        = param->onParameterChanged(sigc::mem_fun(this, &SelectedParameterModAmount::onParamValueChanged));
  }
}

void SelectedParameterModAmount::setRange(float from, float to)
{
  setRangeOrdered(std::min(from, to), std::max(from, to));
}

void SelectedParameterModAmount::setRangeOrdered(float from, float to)
{
  from = std::min(from, 1.0f);
  from = std::max(from, 0.0f);

  to = std::min(to, 1.0f);
  to = std::max(to, 0.0f);

  if(m_from != from || m_to != to)
  {
    m_from = from;
    m_to = to;
    setDirty();
  }
}

bool SelectedParameterModAmount::redraw(FrameBuffer &fb)
{
  if(m_from != 0 || m_to != 0)
  {
    Rect r = getPosition();
    Rect filled = r;
    auto left = round(r.getLeft() + m_from * r.getWidth());
    auto right = round(r.getLeft() + m_to * r.getWidth());
    filled.setLeft(left);
    filled.setWidth(right - left);
    setSliderColor(fb);
    fb.fillRect(filled.getLeft(), filled.getTop(), filled.getWidth(), filled.getHeight());
  }
  return true;
}

void SelectedParameterModAmount::onParamValueChanged(const Parameter *param)
{
  if(const ModulateableParameter *p = dynamic_cast<const ModulateableParameter *>(param))
  {
    auto range = p->getModulationRange(true);
    setRange(range.first, range.second);
  }
  else
  {
    setRange(0, 0);
  }
}

void SelectedParameterModAmount::setSliderColor(FrameBuffer &fb)
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C255);
  else
    fb.setColor(FrameBuffer::Colors::C179);
}
