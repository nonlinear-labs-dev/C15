#pragma once

#include "BooleanSetting.h"

class Settings;

class CrashOnError : public BooleanSetting
{
 public:
  CrashOnError(Settings& parent);
};
