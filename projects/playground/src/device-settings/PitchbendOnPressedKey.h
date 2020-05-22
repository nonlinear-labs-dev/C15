#pragma once

#include "BooleanSetting.h"

class PitchbendOnPressedKey : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  PitchbendOnPressedKey(UpdateDocumentContributor &settings);
  virtual ~PitchbendOnPressedKey();

 private:
  void sendToLPC(SendReason reason) const override;
};
