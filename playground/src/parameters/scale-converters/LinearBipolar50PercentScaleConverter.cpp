#include "LinearBipolar50PercentScaleConverter.h"
#include "dimension/PercentageDimension.h"

LinearBipolar50PercentScaleConverter::LinearBipolar50PercentScaleConverter()
    : LinearScaleConverter(tTcdRange(-8000, 8000), tDisplayRange(-50, 50), PercentageDimension::get())
{
}

LinearBipolar50PercentScaleConverter::~LinearBipolar50PercentScaleConverter()
{
}
