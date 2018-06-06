#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/SignalFlowIndicationSetting.h>
#include <proxies/hwui/HWUI.h>
#include "SetupLabel.h"
#include "SignalFlowIndicatorEditor.h"
#include <proxies/hwui/Font.h>
#include <device-settings/DebugLevel.h>
#include <proxies/hwui/UsageMode.h>

SignalFlowIndicatorEditor::SignalFlowIndicatorEditor () :
    super (Rect (0, 0, 0, 0))
{
  Application::get ().getSettings ()->getSetting<SignalFlowIndicationSetting> ()->onChange (
      mem_fun (this, &SignalFlowIndicatorEditor::onSettingChanged));

}

SignalFlowIndicatorEditor::~SignalFlowIndicatorEditor ()
{
}

Font::Justification SignalFlowIndicatorEditor::getJustification () const
{
  return Font::Justification::Left;
}

bool SignalFlowIndicatorEditor::redraw (FrameBuffer &fb)
{
  super::redraw(fb);
  fb.setColor (FrameBuffer::C179);
  fb.drawRect (getPosition());
  return true;
}

void SignalFlowIndicatorEditor::setBackgroundColor (FrameBuffer &fb) const
{
  fb.setColor (FrameBuffer::C103);
}

void SignalFlowIndicatorEditor::setFontColor (FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::C255);
}

void SignalFlowIndicatorEditor::onSettingChanged (const Setting *s)
{
  if (auto p = dynamic_cast<const SignalFlowIndicationSetting*>(s))
    setText (p->getDisplayString (), 0);
}

bool SignalFlowIndicatorEditor::onButton (Buttons i, bool down, ButtonModifiers modifiers)
{
  return false;
}

bool SignalFlowIndicatorEditor::onRotary (int inc, ButtonModifiers modifiers)
{
  Application::get ().getSettings ()->getSetting<SignalFlowIndicationSetting> ()->toggle();
  return true;
}
