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
    , m_showParameterName(std::bind(&SelectedParamValueWithFrame::showName, this))
{
}

SelectedParamValueWithFrame::~SelectedParamValueWithFrame()
{
}

bool SelectedParamValueWithFrame::redraw(FrameBuffer &fb)
{
  if(m_showParameterName.isPending())
  {
    super::redraw(fb);
  }
  else
  {
    super::Label::redraw(fb);
  }
  getPosition().drawRounded(fb);
  return true;
}

std::shared_ptr<Font> SelectedParamValueWithFrame::getFont() const
{
  return Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
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
    setDirty();
    if(param)
    {
      m_showParameterName.refresh(std::chrono::seconds(1));
    }
  }
}

void SelectedParamValueWithFrame::showName()
{
  if(auto p = Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup()))
  {
    setText(StringAndSuffix { p->getShortName() });
  }
}

void SelectedParamValueWithFrame::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C103);
}

void SelectedParamValueWithFrame::onModifiersChanged(ButtonModifiers mods)
{
  if(m_ignoreNextModifierChange)
  {
    m_ignoreNextModifierChange = false;
  }
  else
  {
    super::onModifiersChanged(mods);
  }
}
