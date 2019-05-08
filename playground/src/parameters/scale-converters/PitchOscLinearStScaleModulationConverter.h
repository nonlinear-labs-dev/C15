#pragma once

#include "LinearScaleConverter.h"

class PitchOscLinearStScaleModulationConverter : public LinearScaleConverter
{
 public:
  PitchOscLinearStScaleModulationConverter();
  virtual ~PitchOscLinearStScaleModulationConverter();

  virtual tControlPositionValue getCoarseDenominator(const QuantizedValue &v) const override;
  virtual tControlPositionValue getFineDenominator(const QuantizedValue &v) const override;
};
