#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class DeviceNameView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  DeviceNameView();
  virtual ~DeviceNameView();

 private:
  void onSettingChanged(const Setting *s);
};
