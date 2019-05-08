#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/PedalType.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/boled/setup/PedalView.h>

PedalView::PedalView(shared_ptr<PedalType> s)
    : base_type("", Rect(0, 0, 0, 0))
{
  s->onChange(mem_fun(this, &PedalView::onSettingChanged));
}

PedalView::~PedalView()
{
}

void PedalView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const PedalType *>(s);
  setText(v->getDisplayString());
}
