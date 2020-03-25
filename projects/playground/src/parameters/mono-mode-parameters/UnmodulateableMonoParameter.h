#pragma once

#include <parameters/Parameter.h>

class UnmodulateableMonoParameter : public Parameter
{
 public:
  UnmodulateableMonoParameter(ParameterGroup *group, const ParameterId &id, const ScaleConverter *scaling,
                              tControlPositionValue def, tControlPositionValue coarseDenominator,
                              tControlPositionValue fineDenominator);

  Layout *createLayout(FocusAndMode focusAndMode) const override;
};
