#include "PitchOscLinearStScaleConverter.h"
#include "dimension/PitchDimension.h"

PitchOscLinearStScaleConverter::PitchOscLinearStScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 15000), tDisplayRange(-20, 130), PitchDimension::get())
{
}

PitchOscLinearStScaleConverter::~PitchOscLinearStScaleConverter()
{
}
