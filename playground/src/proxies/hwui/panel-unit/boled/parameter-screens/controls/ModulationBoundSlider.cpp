#include <Application.h>
#include <parameters/ModulateableParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundSlider.h>

ModulationBoundSlider::ModulationBoundSlider(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      mem_fun(this, &ModulationBoundSlider::onSelectionChanged));
}

ModulationBoundSlider::~ModulationBoundSlider()
{
}

void ModulationBoundSlider::onSelectionChanged(Parameter *, Parameter *newParam)
{
  m_paramChanged.disconnect();

  if(m_param = dynamic_cast<ModulateableParameter *>(newParam))
    m_paramChanged = m_param->onParameterChanged(mem_fun(this, &ModulationBoundSlider::onParameterChanged));
}

void ModulationBoundSlider::onParameterChanged(const Parameter *)
{
  setDirty();
}

bool ModulationBoundSlider::redraw(FrameBuffer &fb)
{
  if(m_param && m_param->getModulationSource() != ModulateableParameter::NONE)
  {
    if(isHighlight())
      fb.setColor(FrameBuffer::C255);
    else
      fb.setColor(FrameBuffer::C103);

    const Rect &pos = getPosition();

    auto y = pos.getTop() + (pos.getHeight() - 3) / 2;
    fb.drawHorizontalLine(pos.getLeft(), y, pos.getWidth());
    fb.drawHorizontalLine(pos.getLeft(), y + 2, pos.getWidth());

    if(getBoundPosition() == BoundPosition::Left)
      fb.drawVerticalLine(pos.getLeft(), pos.getTop(), pos.getHeight() - 1);
    else
      fb.drawVerticalLine(pos.getRight(), pos.getTop(), pos.getHeight() - 1);
  }
  return true;
}
