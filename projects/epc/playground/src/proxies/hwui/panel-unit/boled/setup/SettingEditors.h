#pragma once

#include "MenuEditor.h"
#include "device-settings/Settings.h"
#include "device-settings/Setting.h"
#include <Application.h>

template <typename tSetting> class BooleanSettingsEditor2 : public MenuEditor
{
 public:
  BooleanSettingsEditor2()
  {
    getSetting()->onChange(mem_fun(this, &BooleanSettingsEditor2<tSetting>::onSettingChanged));
  }

  void incSetting(int inc) override
  {
    getSetting()->incDec(inc, false);
  }

  const std::vector<Glib::ustring>& getDisplayStrings() const override
  {
    return getSetting()->enumToDisplayString();
  }

  int getSelectedIndex() const override
  {
    auto enabled = getSetting()->get();
    return enabled ? 0 : 1;
  }

  auto getSetting() const
  {
    return Application::get().getSettings()->getSetting<tSetting>();
  }
};

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
    return static_cast<size_t>(getSetting()->get());
  }

  auto getSetting() const
  {
    return Application::get().getSettings()->getSetting<tSetting>();
  }
};