#pragma once

#include <proxies/hwui/controls/LabelRegular8.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/Setting.h>

template<class tSetting>
class SettingLabel : public LabelRegular8 {
public:
  explicit SettingLabel(const Rect& r) : LabelRegular8(r) {
    m_setting = Application::get().getSettings()->getSetting<tSetting>();
    if(m_setting) {
      m_connection = m_setting->onChange(sigc::mem_fun(this, &SettingLabel<tSetting>::onSettingChanged));
    }
  }

  ~SettingLabel() override {
    m_connection.disconnect();
  }

  void onSettingChanged(const Setting* setting) {
    if(setting) {
      setText({setting->getDisplayString(), 0});
    }
  }

protected:
  std::shared_ptr<tSetting> m_setting;
  sigc::connection m_connection;
};