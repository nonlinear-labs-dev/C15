#include "Linear20To100StScaleConverter.h"
#include "dimension/PitchDimensionCoarse.h"

Linear20To100StScaleConverter::Linear20To100StScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 16000), tDisplayRange(20, 100), PitchDimensionCoarse::get())
{
}

Linear20To100StScaleConverter::~Linear20To100StScaleConverter()
{
}
