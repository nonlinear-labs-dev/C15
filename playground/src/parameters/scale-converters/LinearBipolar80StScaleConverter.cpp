#include "LinearBipolar80StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

LinearBipolar80StScaleConverter::LinearBipolar80StScaleConverter()
    : LinearScaleConverter(tTcdRange(-8000, 8000), tDisplayRange(-80, 80), PitchDimensionCoarse::get())
{
}

LinearBipolar80StScaleConverter::~LinearBipolar80StScaleConverter()
{
}
