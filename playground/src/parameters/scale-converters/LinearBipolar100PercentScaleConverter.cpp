#include "LinearBipolar100PercentScaleConverter.h"
#include "dimension/PercentageDimension.h"

LinearBipolar100PercentScaleConverter::LinearBipolar100PercentScaleConverter()
    : LinearScaleConverter(tTcdRange(-8000, 8000), tDisplayRange(-100, 100), PercentageDimension::get())
{
}

LinearBipolar100PercentScaleConverter::~LinearBipolar100PercentScaleConverter()
{
}
