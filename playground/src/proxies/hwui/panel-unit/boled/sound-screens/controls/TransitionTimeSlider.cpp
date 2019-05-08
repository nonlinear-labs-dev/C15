#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/TransitionTime.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/TransitionTimeSlider.h>

TransitionTimeSlider::TransitionTimeSlider(const Rect &rect)
    : super(rect)
{
  Application::get().getSettings()->getSetting<TransitionTime>()->onChange(
      mem_fun(this, &TransitionTimeSlider::onSettingChanged));
}

TransitionTimeSlider::~TransitionTimeSlider()
{
}

void TransitionTimeSlider::onSettingChanged(const Setting *s)
{
  if(auto p = dynamic_cast<const TransitionTime *>(s))
    setValue(p->get(), false);
}
