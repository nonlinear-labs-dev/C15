#include "LinearBipolar36StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

LinearBipolar36StScaleConverter::LinearBipolar36StScaleConverter()
    : LinearScaleConverter(tTcdRange(-7200, 7200), tDisplayRange(-36, 36), PitchDimensionCoarse::get())
{
}

LinearBipolar36StScaleConverter::~LinearBipolar36StScaleConverter()
{
}
