#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/AftertouchCurve.h>
#include <proxies/hwui/panel-unit/boled/setup/AftertouchView.h>

AftertouchView::AftertouchView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<AftertouchCurve>()->onChange(
      mem_fun(this, &AftertouchView::onSettingChanged));
}

AftertouchView::~AftertouchView()
{
}

void AftertouchView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const AftertouchCurve *>(s);
  setText(v->getDisplayString());
}
