#include "LinearBipolar150StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

LinearBipolar150StScaleConverter::LinearBipolar150StScaleConverter()
    : LinearScaleConverter(tTcdRange(-7500, 7500), tDisplayRange(-150, 150), PitchDimensionCoarse::get())
{
}

LinearBipolar150StScaleConverter::~LinearBipolar150StScaleConverter()
{
}
