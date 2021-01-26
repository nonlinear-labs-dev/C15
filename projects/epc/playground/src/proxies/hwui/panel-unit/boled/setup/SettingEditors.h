#pragma once

#include "MenuEditor.h"
#include "device-settings/Settings.h"
#include "device-settings/Setting.h"
#include <Application.h>
#include <device-settings/BooleanSetting.h>

//fix for boolean setting
template <typename tSetting> class EnumSettingEditor : public MenuEditor
{
 public:
  EnumSettingEditor()
  {
    getSetting()->onChange(mem_fun(this, &EnumSettingEditor<tSetting>::onSettingChanged));
  }

  void incSetting(int inc) override
  {
    getSetting()->incDec(inc, false);
  }

  const std::vector<Glib::ustring>& getDisplayStrings() const override
  {
    return getSetting()->getDisplayStrings();
  }

  int getSelectedIndex() const override
  {
    return static_cast<size_t>(getSetting()->getEnumIndex());
  }

  auto getSetting() const
  {
    return Application::get().getSettings()->getSetting<tSetting>();
  }
};