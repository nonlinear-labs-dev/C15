#pragma once

#include "HostAPDSetting.h"

class Settings;

class SSID : public HostAPDSetting
{
 private:
  typedef HostAPDSetting super;

 public:
  SSID(Settings &parent);
  virtual ~SSID();
};
