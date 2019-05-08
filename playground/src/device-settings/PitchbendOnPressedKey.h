#pragma once

#include "BooleanSetting.h"

class PitchbendOnPressedKey : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  PitchbendOnPressedKey(Settings &settings);
  virtual ~PitchbendOnPressedKey();

 private:
  void sendToLPC() const override;
};
