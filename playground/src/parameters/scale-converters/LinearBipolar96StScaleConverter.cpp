#include "LinearBipolar96StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

LinearBipolar96StScaleConverter::LinearBipolar96StScaleConverter()
    : LinearScaleConverter(tTcdRange(-6000, 6000), tDisplayRange(-96, 96), PitchDimensionCoarse::get())
{
}

LinearBipolar96StScaleConverter::~LinearBipolar96StScaleConverter()
{
}
