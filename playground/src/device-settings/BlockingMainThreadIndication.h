#pragma once
#include <device-settings/BooleanSetting.h>

class BlockingMainThreadIndication : public BooleanSetting
{
 public:
  using BooleanSetting::BooleanSetting;
};
