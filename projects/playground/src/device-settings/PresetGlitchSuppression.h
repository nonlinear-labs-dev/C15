#pragma once

#include "BooleanSetting.h"

class PresetGlitchSuppression : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  PresetGlitchSuppression(UpdateDocumentContributor &settings);
  virtual ~PresetGlitchSuppression();

 private:
  void sendToLPC(SendReason reason) const override;
};
