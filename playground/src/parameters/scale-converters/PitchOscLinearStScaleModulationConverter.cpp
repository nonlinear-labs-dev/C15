#include "PitchOscLinearStScaleModulationConverter.h"
#include "dimension/PitchDimension.h"

PitchOscLinearStScaleModulationConverter::PitchOscLinearStScaleModulationConverter()
    : LinearScaleConverter(tTcdRange(0, 15000), tDisplayRange(-150, 150), PitchDimension::get())
{
}

PitchOscLinearStScaleModulationConverter::~PitchOscLinearStScaleModulationConverter()
{
}

tControlPositionValue PitchOscLinearStScaleModulationConverter::getCoarseDenominator(const QuantizedValue &v) const
{
  return 150;
}

tControlPositionValue PitchOscLinearStScaleModulationConverter::getFineDenominator(const QuantizedValue &v) const
{
  return 7500;
}
