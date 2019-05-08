#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/base-unit/soled/SelectedParamValueWithFrame.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/Oleds.h>

SelectedParamValueWithFrame::SelectedParamValueWithFrame(const Rect &rect)
    : super(rect)
    , m_showParameterName(bind(&SelectedParamValueWithFrame::showName, this))
{
}

SelectedParamValueWithFrame::~SelectedParamValueWithFrame()
{
}

bool SelectedParamValueWithFrame::redraw(FrameBuffer &fb)
{
  super::redraw(fb);
  getPosition().drawRounded(fb);
  return true;
}

shared_ptr<Font> SelectedParamValueWithFrame::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int SelectedParamValueWithFrame::getFontHeight() const
{
  return 8;
}

void SelectedParamValueWithFrame::onParameterSelected(Parameter *parameter)
{
  m_ignoreNextValueChange = true;
  super::onParameterSelected(parameter);
  showName();
}

void SelectedParamValueWithFrame::onParamValueChanged(const Parameter *param)
{
  if(m_ignoreNextValueChange)
  {
    m_ignoreNextValueChange = false;
  }
  else
  {
    super::onParamValueChanged(param);

    if(param)
    {
      m_showParameterName.refresh(std::chrono::seconds(1));
    }
  }
}

void SelectedParamValueWithFrame::showName()
{
  if(auto p = Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter())
    setText(p->getShortName());
}

void SelectedParamValueWithFrame::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C103);
}

void SelectedParamValueWithFrame::onModifiersChanged(ButtonModifiers mods)
{
  if(m_ignoreNextModifierChange)
    m_ignoreNextModifierChange = false;
  else
    super::onModifiersChanged(mods);
}
