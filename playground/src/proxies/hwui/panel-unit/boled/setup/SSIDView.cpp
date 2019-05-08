#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/SSID.h>
#include <proxies/hwui/panel-unit/boled/setup/SSIDView.h>

SSIDView::SSIDView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<SSID>()->onChange(mem_fun(this, &SSIDView::onSettingChanged));
}

SSIDView::~SSIDView()
{
}

void SSIDView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const SSID *>(s);
  setText(v->get());
}
