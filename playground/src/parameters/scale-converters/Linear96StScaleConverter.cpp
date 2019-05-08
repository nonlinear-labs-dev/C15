#include "Linear96StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

Linear96StScaleConverter::Linear96StScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 9600), tDisplayRange(0, 96), PitchDimensionCoarse::get())
{
}

Linear96StScaleConverter::~Linear96StScaleConverter()
{
}
