#include "LinearBipolar120StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

LinearBipolar120StScaleConverter::LinearBipolar120StScaleConverter()
    : LinearScaleConverter(tTcdRange(-7500, 7500), tDisplayRange(-120, 120), PitchDimensionCoarse::get())
{
}

LinearBipolar120StScaleConverter::~LinearBipolar120StScaleConverter()
{
}
