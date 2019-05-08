#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/EditSmoothingTime.h>
#include <proxies/hwui/panel-unit/boled/setup/EditSmoothingTimeView.h>

EditSmoothingTimeView::EditSmoothingTimeView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<EditSmoothingTime>()->onChange(
      mem_fun(this, &EditSmoothingTimeView::onSettingChanged));
}

EditSmoothingTimeView::~EditSmoothingTimeView()
{
}

void EditSmoothingTimeView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const EditSmoothingTime *>(s);
  setText(v->getDisplayString());
}
