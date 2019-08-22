#pragma once
#include "../base/EventSource.h"
#include <device-settings/Setting.h>
#include <device-settings/Settings.h>
#include <Application.h>

namespace DescriptiveLayouts
{
  template <typename T, class tSetting> class SettingEvent : public EventSource<T>
  {
   public:
    SettingEvent()
    {
      auto setting = Application::get().getSettings()->getSetting<tSetting>();
      assert(setting.get() != nullptr);
      m_settingConnection = setting->onChange(sigc::mem_fun(this, &SettingEvent<T, tSetting>::onSettingChanged));
    }

    ~SettingEvent()
    {
      m_settingConnection.disconnect();
    }

    virtual void onSettingChanged(const Setting* s)
    {
      if(auto ss = dynamic_cast<const tSetting*>(s))
      {
        EventSource<T>::setValue((T) ss->get());
      }
    }

   protected:
    sigc::connection m_settingConnection;
  };
}
