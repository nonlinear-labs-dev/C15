#pragma once
#include <device-settings/BooleanSetting.h>

class EnableHighVelocityCC : public BooleanSetting
{
 public:
  explicit EnableHighVelocityCC(Settings& s);
};
