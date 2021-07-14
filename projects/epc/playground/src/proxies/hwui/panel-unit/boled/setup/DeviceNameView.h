#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class Setting;

class DeviceNameView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  DeviceNameView();
  ~DeviceNameView() override;

 private:
  void onSettingChanged(const Setting *s);
};
