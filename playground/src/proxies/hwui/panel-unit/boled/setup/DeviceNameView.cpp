#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/DeviceName.h>
#include <proxies/hwui/panel-unit/boled/setup/DeviceNameView.h>

DeviceNameView::DeviceNameView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<DeviceName>()->onChange(
      mem_fun(this, &DeviceNameView::onSettingChanged));
}

DeviceNameView::~DeviceNameView()
{
}

void DeviceNameView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const DeviceName *>(s);
  setText(v->get());
}
