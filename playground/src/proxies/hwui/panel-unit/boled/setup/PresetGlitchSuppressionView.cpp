#include <Application.h>
#include <device-settings/PresetGlitchSuppression.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/boled/setup/PresetGlitchSuppressionView.h>

PresetGlitchSuppressionView::PresetGlitchSuppressionView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<PresetGlitchSuppression>()->onChange(
      mem_fun(this, &PresetGlitchSuppressionView::onSettingChanged));
}

PresetGlitchSuppressionView::~PresetGlitchSuppressionView()
{
}

void PresetGlitchSuppressionView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const PresetGlitchSuppression *>(s);
  setText(v->getDisplayString());
}
