#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/EncoderAcceleration.h>
#include <proxies/hwui/panel-unit/boled/setup/EncoderAccelerationEditor.h>
#include <proxies/hwui/HWUI.h>
#include "SetupLabel.h"

EncoderAccelerationEditor::EncoderAccelerationEditor()
    : super(Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<EncoderAcceleration>()->onChange(
      mem_fun(this, &EncoderAccelerationEditor::onSettingChanged));
}

EncoderAccelerationEditor::~EncoderAccelerationEditor()
{
}

bool EncoderAccelerationEditor::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  fb.setColor(FrameBuffer::C179);
  fb.drawRect(getPosition());
  return true;
}

void EncoderAccelerationEditor::setBackgroundColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::C103);
}

Font::Justification EncoderAccelerationEditor::getJustification() const
{
  return Font::Justification::Left;
}

void EncoderAccelerationEditor::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::C255);
}

void EncoderAccelerationEditor::onSettingChanged(const Setting *s)
{
  if(auto p = dynamic_cast<const EncoderAcceleration *>(s))
    setText(p->getDisplayString());
}

bool EncoderAccelerationEditor::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  return false;
}

bool EncoderAccelerationEditor::onRotary(int inc, ButtonModifiers modifiers)
{
  Application::get().getSettings()->getSetting<EncoderAcceleration>()->incDec(inc, modifiers);
  return true;
}
