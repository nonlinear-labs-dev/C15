#pragma once

#include "BooleanSetting.h"

class Settings;

class CrashOnError : public BooleanSetting
{
 public:
  explicit CrashOnError(Settings& parent);
};
