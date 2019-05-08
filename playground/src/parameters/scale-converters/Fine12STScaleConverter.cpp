#include "Fine12STScaleConverter.h"
#include "dimension/PitchDimensionFine.h"

Fine12STScaleConverter::Fine12STScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 12000), tDisplayRange(0, 12), PitchDimensionFine::get())
{
}

Fine12STScaleConverter::~Fine12STScaleConverter()
{
}
