#include "FineBipolar12STScaleConverter.h"
#include "dimension/PitchDimensionFine.h"

FineBipolar12STScaleConverter::FineBipolar12STScaleConverter()
    : LinearScaleConverter(tTcdRange(-6000, 6000), tDisplayRange(-12, 12), PitchDimensionFine::get())
{
}

FineBipolar12STScaleConverter::~FineBipolar12STScaleConverter()
{
}
