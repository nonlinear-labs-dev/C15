#include "PitchCombLinearStScaleConverter.h"
#include "dimension/PitchDimension.h"

PitchCombLinearStScaleConverter::PitchCombLinearStScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 12000), tDisplayRange(0, 120), PitchDimension::get())
{
}

PitchCombLinearStScaleConverter::~PitchCombLinearStScaleConverter()
{
}
