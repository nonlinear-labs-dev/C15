#pragma once
#include <device-settings/BooleanSetting.h>

class KioskModeSetting : public BooleanSetting
{
 public:
  KioskModeSetting(Settings& settings);
  virtual ~KioskModeSetting();

  bool set(Glib::ustring value);
  bool setBool(bool value);
  void setSetting(Initiator initiator, const Glib::ustring& text) override;
};
