#include "LinearBipolar140StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

LinearBipolar140StScaleConverter::LinearBipolar140StScaleConverter()
    : LinearScaleConverter(tTcdRange(-7500, 7500), tDisplayRange(-140, 140), PitchDimensionCoarse::get())
{
}

LinearBipolar140StScaleConverter::~LinearBipolar140StScaleConverter()
{
}
