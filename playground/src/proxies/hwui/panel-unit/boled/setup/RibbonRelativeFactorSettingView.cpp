#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/RibbonRelativeFactor.h>
#include <proxies/hwui/panel-unit/boled/setup/RibbonRelativeFactorSettingView.h>

RibbonRelativeFactorSettingView::RibbonRelativeFactorSettingView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<RibbonRelativeFactor>()->onChange(
      mem_fun(this, &RibbonRelativeFactorSettingView::onSettingChanged));
}

RibbonRelativeFactorSettingView::~RibbonRelativeFactorSettingView()
{
}

void RibbonRelativeFactorSettingView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const RibbonRelativeFactor *>(s);
  setText(v->getDisplayString());
}
