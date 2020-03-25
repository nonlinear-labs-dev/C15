#include "LinearBipolar96StScaleConverterCoarse.h"
#include "dimension/PitchDimensionCoarse.h"

LinearBipolar96StScaleConverterCoarse::LinearBipolar96StScaleConverterCoarse()
    : LinearScaleConverter(tTcdRange(-6000, 6000), tDisplayRange(-96, 96), PitchDimensionCoarse::get())
{
}

LinearBipolar96StScaleConverterCoarse::~LinearBipolar96StScaleConverterCoarse()
{
}
