#pragma once
#include "ModulateableParameter.h"

class ModulateableParameterWithUnusualModDenominator : public ModulateableParameter
{
 public:
  ModulateableParameterWithUnusualModDenominator(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling,
                                                 tDisplayValue def, tControlPositionValue coarseDenominator,
                                                 tControlPositionValue fineDenominator,
                                                 tControlPositionValue modAmountCoarseDenom,
                                                 tControlPositionValue modAmountFineDenom);

  double getModulationAmountFineDenominator() const override;

  double getModulationAmountCoarseDenominator() const override;

 private:
  tControlPositionValue m_fineModAmountDenominator;
  tControlPositionValue m_coarseModAmountDenominator;
};
