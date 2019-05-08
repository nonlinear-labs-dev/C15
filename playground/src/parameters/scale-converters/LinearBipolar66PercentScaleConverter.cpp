#include "LinearBipolar66PercentScaleConverter.h"
#include "dimension/PercentageDimension.h"

LinearBipolar66PercentScaleConverter::LinearBipolar66PercentScaleConverter()
    : LinearScaleConverter(tTcdRange(-6600, 6600), tDisplayRange(-66, 66), PercentageDimension::get())
{
}

LinearBipolar66PercentScaleConverter::~LinearBipolar66PercentScaleConverter()
{
}
