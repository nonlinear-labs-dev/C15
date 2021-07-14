#pragma once
#include <parameters/ModulateableParameterWithUnusualModUnit.h>

class VoiceGroupMasterModulateableParameterWithUnusualModUnit : public ModulateableParameterWithUnusualModUnit
{
 public:
  VoiceGroupMasterModulateableParameterWithUnusualModUnit(ParameterGroup* group, const ParameterId& id,
                                                          const ScaleConverter* scale, const ScaleConverter* modScale,
                                                          tControlPositionValue def, int coarse, int fine);
  Layout* createLayout(FocusAndMode focusAndMode) const override;
};
