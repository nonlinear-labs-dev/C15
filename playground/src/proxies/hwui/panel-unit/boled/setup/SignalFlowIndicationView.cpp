#include <proxies/hwui/panel-unit/boled/setup/SignalFlowIndicationView.h>
#include <device-settings/Settings.h>

SignalFlowIndicationView::SignalFlowIndicationView()
    : SetupLabel("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<SignalFlowIndicationSetting>()->onChange(
      mem_fun(this, &SignalFlowIndicationView::onSettingChanged));
}

SignalFlowIndicationView::~SignalFlowIndicationView()
{
}

void SignalFlowIndicationView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const SignalFlowIndicationSetting *>(s);
  setText(v->getDisplayString());
}
