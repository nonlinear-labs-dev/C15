#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/RandomizeAmount.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/RandomizeAmountSlider.h>

RandomizeAmountSlider::RandomizeAmountSlider(const Rect &rect)
    : super(rect)
{
  Application::get().getSettings()->getSetting<RandomizeAmount>()->onChange(
      mem_fun(this, &RandomizeAmountSlider::onSettingChanged));
}

RandomizeAmountSlider::~RandomizeAmountSlider()
{
}

void RandomizeAmountSlider::onSettingChanged(const Setting *s)
{
  if(auto p = dynamic_cast<const RandomizeAmount *>(s))
    setValue(p->get(), false);
}
