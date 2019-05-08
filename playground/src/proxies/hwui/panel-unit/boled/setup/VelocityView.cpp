#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/VelocityCurve.h>
#include <proxies/hwui/panel-unit/boled/setup/VelocityView.h>

VelocityView::VelocityView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<VelocityCurve>()->onChange(
      mem_fun(this, &VelocityView::onSettingChanged));
}

VelocityView::~VelocityView()
{
}

void VelocityView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const VelocityCurve *>(s);
  setText(v->getDisplayString());
}
