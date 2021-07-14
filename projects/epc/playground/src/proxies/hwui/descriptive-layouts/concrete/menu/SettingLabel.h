#pragma once

#include <proxies/hwui/controls/LabelRegular8.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/Setting.h>

template <class tSetting> class SettingLabel : public Label
{
 public:
  explicit SettingLabel(const Rect& r)
      : Label(r)
  {
    m_setting = Application::get().getSettings()->getSetting<tSetting>();
    if(m_setting)
    {
      m_connection = m_setting->onChange(sigc::mem_fun(this, &SettingLabel<tSetting>::onSettingChanged));
    }
  }

  ~SettingLabel() override
  {
    m_connection.disconnect();
  }

  void onSettingChanged(const Setting* setting)
  {
    if(setting)
    {
      setText(StringAndSuffix { setting->getDisplayString(), 0 });
    }
  }

 private:
  std::shared_ptr<tSetting> m_setting;
  sigc::connection m_connection;
};
