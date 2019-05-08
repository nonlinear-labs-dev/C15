#include "Linear24To120StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

Linear24To120StScaleConverter::Linear24To120StScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 9600), tDisplayRange(24, 120), PitchDimensionCoarse::get())
{
}

Linear24To120StScaleConverter::~Linear24To120StScaleConverter()
{
}
