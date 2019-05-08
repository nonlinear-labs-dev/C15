#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/Passphrase.h>
#include <proxies/hwui/panel-unit/boled/setup/PassphraseView.h>

PassphraseView::PassphraseView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getSettings()->getSetting<Passphrase>()->onChange(
      mem_fun(this, &PassphraseView::onSettingChanged));
}

PassphraseView::~PassphraseView()
{
}

void PassphraseView::onSettingChanged(const Setting *s)
{
  auto v = dynamic_cast<const Passphrase *>(s);
  setText(v->get());
}
