#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/EncoderAcceleration.h>
#include <proxies/hwui/panel-unit/boled/setup/EncoderAccelerationView.h>

EncoderAccelerationView::EncoderAccelerationView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<EncoderAcceleration>()->onChange(
      mem_fun(this, &EncoderAccelerationView::onSettingChanged));
}

EncoderAccelerationView::~EncoderAccelerationView()
{
}

void EncoderAccelerationView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const EncoderAcceleration *>(s);
  setText(v->getDisplayString());
}
