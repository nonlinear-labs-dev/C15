#pragma once

#include "BooleanSetting.h"

class AutoLoadSelectedPreset : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  AutoLoadSelectedPreset(Settings& settings);
  virtual ~AutoLoadSelectedPreset();

  virtual bool set(tEnum m) override;
  void cheat(tEnum m);

  AutoLoadSelectedPreset(const AutoLoadSelectedPreset& other) = delete;
  AutoLoadSelectedPreset& operator=(const AutoLoadSelectedPreset&) = delete;
};
