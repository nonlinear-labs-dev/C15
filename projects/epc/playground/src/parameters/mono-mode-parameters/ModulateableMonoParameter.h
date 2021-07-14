#pragma once
#include <parameters/ModulateableParameter.h>

class ModulateableMonoParameter : public ModulateableParameter
{
 public:
  ModulateableMonoParameter(ParameterGroup *group, const ParameterId &id, const ScaleConverter *scaling,
                            tControlPositionValue def, tControlPositionValue coarseDenominator,
                            tControlPositionValue fineDenominator);

  Layout *createLayout(FocusAndMode focusAndMode) const override;
};
