#pragma once

#include "LinearScaleConverter.h"

class PitchCombLinearStModulationScaleConverter : public LinearScaleConverter
{
 public:
  PitchCombLinearStModulationScaleConverter();
  ~PitchCombLinearStModulationScaleConverter() override;

  [[nodiscard]] tControlPositionValue getCoarseDenominator(const QuantizedValue &v) const override;
  [[nodiscard]] tControlPositionValue getFineDenominator(const QuantizedValue &v) const override;
};
