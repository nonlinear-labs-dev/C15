#include "LinearBipolar200PercentScaleConverter.h"
#include "dimension/PercentageDimension.h"

LinearBipolar200PercentScaleConverter::LinearBipolar200PercentScaleConverter()
    : LinearScaleConverter(tTcdRange(-2000, 2000), tDisplayRange(-200, 200), PercentageDimension::get())
{
}

LinearBipolar200PercentScaleConverter::~LinearBipolar200PercentScaleConverter()
{
}

tControlPositionValue LinearBipolar200PercentScaleConverter::getCoarseDenominator(const QuantizedValue &v) const
{
  return ScaleConverter::getCoarseDenominator(v) * 2;
}

tControlPositionValue LinearBipolar200PercentScaleConverter::getFineDenominator(const QuantizedValue &v) const
{
  return ScaleConverter::getFineDenominator(v) * 2;
}
