#pragma once
#include <device-settings/BooleanSetting.h>

class Enable14BitSupport : public BooleanSetting
{
 public:
  explicit Enable14BitSupport(Settings& s);
};
