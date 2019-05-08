#include "LinearBipolar60StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

LinearBipolar60StScaleConverter::LinearBipolar60StScaleConverter()
    : LinearScaleConverter(tTcdRange(-6000, 6000), tDisplayRange(-60, 60), PitchDimensionCoarse::get())
{
}

LinearBipolar60StScaleConverter::~LinearBipolar60StScaleConverter()
{
}
