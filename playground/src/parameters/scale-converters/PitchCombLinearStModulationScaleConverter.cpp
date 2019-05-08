#include "PitchCombLinearStModulationScaleConverter.h"
#include "dimension/PitchDimension.h"

PitchCombLinearStModulationScaleConverter::PitchCombLinearStModulationScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 12000), tDisplayRange(-120, 120), PitchDimension::get())
{
}

PitchCombLinearStModulationScaleConverter::~PitchCombLinearStModulationScaleConverter()
{
}

tControlPositionValue PitchCombLinearStModulationScaleConverter::getCoarseDenominator(const QuantizedValue &v) const
{
  return 120;
}

tControlPositionValue PitchCombLinearStModulationScaleConverter::getFineDenominator(const QuantizedValue &v) const
{
  return 6000;
}
