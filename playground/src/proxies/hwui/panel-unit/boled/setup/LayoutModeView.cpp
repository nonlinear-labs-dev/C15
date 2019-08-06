#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/LayoutMode.h>
#include <proxies/hwui/panel-unit/boled/setup/LayoutModeView.h>

LayoutModeView::LayoutModeView()
    : super("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<LayoutMode>()->onChange(
      mem_fun(this, &LayoutModeView::onSettingChanged));
}

LayoutModeView::~LayoutModeView() = default;

void LayoutModeView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const LayoutMode *>(s);
  setText(v->getDisplayString());
}
