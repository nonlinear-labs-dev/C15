#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class DeviceInformationItem;

class FreeInternalMemoryView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  FreeInternalMemoryView();
  virtual ~FreeInternalMemoryView();

 private:
  void onSettingChanged(const DeviceInformationItem *s);
};
