#include "Linear24StScaleConverter.h"
#include "dimension/PitchDimension.h"

Linear24StScaleConverter::Linear24StScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 12000), tDisplayRange(0, 24), PitchDimension::get())
{
}

Linear24StScaleConverter::~Linear24StScaleConverter()
{
}
