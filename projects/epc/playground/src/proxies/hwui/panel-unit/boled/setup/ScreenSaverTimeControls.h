#pragma once
#include "SetupLabel.h"
#include "SetupEditor.h"
#include "MenuEditor.h"
#include "device-settings/ScreenSaverTimeoutSetting.h"

class ScreenSaverTimeView : public SetupLabel
{
 public:
  ScreenSaverTimeView();
  void onSettingChanged(const Setting *s);
};

class ScreenSaverTimeEditor : public MenuEditor
{
 private:
  typedef MenuEditor super;

 public:
  ScreenSaverTimeEditor();
  ~ScreenSaverTimeEditor() override;

 protected:
  void incSetting(int inc) override;
  const std::vector<Glib::ustring> &getDisplayStrings() const override;
  int getSelectedIndex() const override;

 private:
  static ScreenSaverTimeoutSetting *getSetting();
};