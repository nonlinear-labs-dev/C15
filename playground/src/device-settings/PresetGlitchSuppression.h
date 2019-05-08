#pragma once

#include "BooleanSetting.h"

class PresetGlitchSuppression : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  PresetGlitchSuppression(Settings &settings);
  virtual ~PresetGlitchSuppression();

 private:
  void sendToLPC() const override;
};
