#pragma once

#include "LinearScaleConverter.h"

class PitchCombLinearStModulationScaleConverter : public LinearScaleConverter
{
 public:
  PitchCombLinearStModulationScaleConverter();
  virtual ~PitchCombLinearStModulationScaleConverter();

  virtual tControlPositionValue getCoarseDenominator(const QuantizedValue &v) const override;
  virtual tControlPositionValue getFineDenominator(const QuantizedValue &v) const override;
};
