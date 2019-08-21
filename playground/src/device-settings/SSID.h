#pragma once

#include "HostAPDSetting.h"

class Settings;

class SSID : public HostAPDSetting
{
 private:
  typedef HostAPDSetting super;
public:
  ustring getDisplayString() const override;

public:
  SSID(Settings &parent);
  virtual ~SSID();
};
