#include "dimension/LevelDimension.h"
#include "LinearBipolar24DbScaleConverter.h"

LinearBipolar24DbScaleConverter::LinearBipolar24DbScaleConverter()
    : LinearScaleConverter(tTcdRange(-7200, 7200), tDisplayRange(-24, 24), LevelDimension::get())
{
}

LinearBipolar24DbScaleConverter::~LinearBipolar24DbScaleConverter()
{
}
