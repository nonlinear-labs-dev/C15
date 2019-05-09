#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/RibbonRelativeFactor.h>
#include <proxies/hwui/panel-unit/boled/setup/RibbonRelativeFactorSettingEditor.h>
#include <proxies/hwui/HWUI.h>
#include "SetupLabel.h"

RibbonRelativeFactorSettingEditor::RibbonRelativeFactorSettingEditor()
    : super(Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<RibbonRelativeFactor>()->onChange(
      mem_fun(this, &RibbonRelativeFactorSettingEditor::onSettingChanged));
}

RibbonRelativeFactorSettingEditor::~RibbonRelativeFactorSettingEditor()
{
}

Font::Justification RibbonRelativeFactorSettingEditor::getJustification() const
{
  return Font::Justification::Left;
}

bool RibbonRelativeFactorSettingEditor::redraw(FrameBuffer &fb)
{
  super::redraw(fb);
  fb.setColor(FrameBuffer::C179);
  fb.drawRect(getPosition());
  return true;
}

void RibbonRelativeFactorSettingEditor::setBackgroundColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::C103);
}

void RibbonRelativeFactorSettingEditor::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::C255);
}

void RibbonRelativeFactorSettingEditor::onSettingChanged(const Setting *s)
{
  if(auto p = dynamic_cast<const RibbonRelativeFactor *>(s))
    setText(p->getDisplayString());
}

bool RibbonRelativeFactorSettingEditor::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  return false;
}

bool RibbonRelativeFactorSettingEditor::onRotary(int inc, ButtonModifiers modifiers)
{
  Application::get().getSettings()->getSetting<RibbonRelativeFactor>()->incDec(inc, modifiers);
  return true;
}
