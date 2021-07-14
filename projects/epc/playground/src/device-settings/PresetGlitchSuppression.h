#pragma once

#include "BooleanSetting.h"

class PresetGlitchSuppression : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  explicit PresetGlitchSuppression(UpdateDocumentContributor &settings);
  ~PresetGlitchSuppression() override;

  void syncExternals(SendReason reason) const override;
};
