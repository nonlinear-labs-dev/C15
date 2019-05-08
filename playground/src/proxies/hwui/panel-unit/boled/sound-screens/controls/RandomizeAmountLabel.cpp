#include <Application.h>
#include <device-settings/RandomizeAmount.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/RandomizeAmountLabel.h>
#include <proxies/hwui/HWUI.h>

RandomizeAmountLabel::RandomizeAmountLabel(const Rect &rect)
    : super(rect)
{
  Application::get().getSettings()->getSetting<RandomizeAmount>()->onChange(
      sigc::mem_fun(this, &RandomizeAmountLabel::onSettingChanged));

  Application::get().getHWUI()->onModifiersChanged(
      sigc::hide(sigc::mem_fun(this, &RandomizeAmountLabel::onModifiersChanged)));
}

RandomizeAmountLabel::~RandomizeAmountLabel()
{
}

void RandomizeAmountLabel::onSettingChanged(const Setting *s)
{
  auto displayString = (((const RandomizeAmount *) s)->getDisplayString());

  if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
  {
    setText(displayString + " F", 2);
  }
  else
  {
    setText(displayString);
  }
}

void RandomizeAmountLabel::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C103);
}

void RandomizeAmountLabel::onModifiersChanged()
{
  onSettingChanged(Application::get().getSettings()->getSetting<RandomizeAmount>().get());
}
