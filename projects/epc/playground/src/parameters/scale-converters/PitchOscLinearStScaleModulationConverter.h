#pragma once

#include "LinearScaleConverter.h"

class PitchOscLinearStScaleModulationConverter : public LinearScaleConverter
{
 public:
  PitchOscLinearStScaleModulationConverter();
  ~PitchOscLinearStScaleModulationConverter() override;

  [[nodiscard]] tControlPositionValue getCoarseDenominator(const QuantizedValue &v) const override;
  [[nodiscard]] tControlPositionValue getFineDenominator(const QuantizedValue &v) const override;
};
