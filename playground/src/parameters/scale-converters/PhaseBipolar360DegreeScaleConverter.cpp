#include "PhaseBipolar360DegreeScaleConverter.h"
#include "dimension/DegreeDimensionCoarse.h"

PhaseBipolar360DegreeScaleConverter::PhaseBipolar360DegreeScaleConverter()
    : LinearScaleConverter(tTcdRange(-7200, 7200), tDisplayRange(-360, 360), DegreeDimensionCoarse::get())
{
}

PhaseBipolar360DegreeScaleConverter::~PhaseBipolar360DegreeScaleConverter()
{
}
