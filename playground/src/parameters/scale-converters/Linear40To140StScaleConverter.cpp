#include "Linear40To140StScaleConverter.h"
#include "dimension/PitchDimension.h"

Linear40To140StScaleConverter::Linear40To140StScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 10000), tDisplayRange(40, 140), PitchDimension::get())
{
}

Linear40To140StScaleConverter::~Linear40To140StScaleConverter()
{
}
