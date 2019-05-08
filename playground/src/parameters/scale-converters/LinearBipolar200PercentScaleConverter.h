#pragma once

#include "LinearScaleConverter.h"

class LinearBipolar200PercentScaleConverter : public LinearScaleConverter
{
 public:
  LinearBipolar200PercentScaleConverter();
  virtual ~LinearBipolar200PercentScaleConverter();

  virtual tControlPositionValue getCoarseDenominator(const QuantizedValue &v) const override;
  virtual tControlPositionValue getFineDenominator(const QuantizedValue &v) const override;
};
