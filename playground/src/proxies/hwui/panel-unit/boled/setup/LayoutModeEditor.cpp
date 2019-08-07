#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/LayoutMode.h>
#include <proxies/hwui/panel-unit/boled/setup/LayoutModeEditor.h>
#include <proxies/hwui/HWUI.h>
#include "SetupLabel.h"

LayoutModeEditor::LayoutModeEditor()
    : super(Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<LayoutMode>()->onChange(
      mem_fun(this, &LayoutModeEditor::onSettingChanged));
}

LayoutModeEditor::~LayoutModeEditor()
{
}

bool LayoutModeEditor::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  fb.setColor(FrameBuffer::C179);
  fb.drawRect(getPosition());
  return true;
}

void LayoutModeEditor::setBackgroundColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::C103);
}

Font::Justification LayoutModeEditor::getJustification() const
{
  return Font::Justification::Left;
}

void LayoutModeEditor::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::C255);
}

void LayoutModeEditor::onSettingChanged(const Setting *s)
{
  if(auto p = dynamic_cast<const LayoutMode *>(s))
    setText(p->getDisplayString());
}

bool LayoutModeEditor::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  switch(i)
  {
    case Buttons::BUTTON_DEC:
    case Buttons::ROTARY_MINUS:
      if(down)
        Application::get().getSettings()->getSetting<LayoutMode>()->inc(-1, true);
      return true;
    case Buttons::BUTTON_INC:
    case Buttons::ROTARY_PLUS:
      if(down)
        Application::get().getSettings()->getSetting<LayoutMode>()->inc(1, true);
      return true;
    default:
      return false;
  }
}

bool LayoutModeEditor::onRotary(int inc, ButtonModifiers modifiers)
{
  Application::get().getSettings()->getSetting<LayoutMode>()->inc(modifiers[SHIFT] ? -inc : inc, true);
  return true;
}
