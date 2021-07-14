#pragma once
#include <device-settings/BooleanSetting.h>

class AutoStartRecorderSetting : public BooleanSetting
{
 public:
  explicit AutoStartRecorderSetting(Settings& s);
};
