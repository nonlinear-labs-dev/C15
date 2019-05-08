#include "LinearBipolar33PercentScaleConverter.h"
#include "dimension/PercentageDimension.h"

LinearBipolar33PercentScaleConverter::LinearBipolar33PercentScaleConverter()
    : LinearScaleConverter(tTcdRange(-6600, 6600), tDisplayRange(-33, 33), PercentageDimension::get())
{
}

LinearBipolar33PercentScaleConverter::~LinearBipolar33PercentScaleConverter()
{
}
