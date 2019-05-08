#include "Linear80To140StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

Linear80To140StScaleConverter::Linear80To140StScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 12000), tDisplayRange(80, 140), PitchDimensionCoarse::get())
{
}

Linear80To140StScaleConverter::~Linear80To140StScaleConverter()
{
}
