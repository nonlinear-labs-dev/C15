#include <device-settings/WifiSetting.h>
#include <Application.h>
#include "WiFiSettingEditor.h"
#include <device-settings/Settings.h>

WiFiSettingEditor::WiFiSettingEditor()
{
  Application::get().getSettings()->getSetting<WifiSetting>()->onChange(
      mem_fun(this, &WiFiSettingEditor::onSettingChanged));
}

WiFiSettingEditor::~WiFiSettingEditor() = default;

void WiFiSettingEditor::incSetting(int)
{
  auto setting = Application::get().getSettings()->getSetting<WifiSetting>();
  auto s = setting->get();

  if(s == WifiSettings::Disabled)
    setting->set(WifiSettings::Enabled);
  else if(s == WifiSettings::Enabled)
    setting->set(WifiSettings::Disabled);
}

const std::vector<Glib::ustring> &WiFiSettingEditor::getDisplayStrings() const
{
  static std::vector<Glib::ustring> s { "Disabled", "Enabled" };
  return s;
}

int WiFiSettingEditor::getSelectedIndex() const
{
  auto s = Application::get().getSettings()->getSetting<WifiSetting>()->get();
  return s == WifiSettings::Disabled ? 0 : 1;
}
