#include <Application.h>
#include <device-settings/BenderCurve.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/boled/setup/BenderCurveView.h>

BenderCurveView::BenderCurveView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<BenderCurve>()->onChange(
      mem_fun(this, &BenderCurveView::onSettingChanged));
}

BenderCurveView::~BenderCurveView()
{
}

void BenderCurveView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const BenderCurve *>(s);
  setText(v->getDisplayString());
}
