#include "dimension/LevelDimension.h"
#include "LinearBipolar50DbScaleConverter.h"

LinearBipolar50DbScaleConverter::LinearBipolar50DbScaleConverter()
    : LinearScaleConverter(tTcdRange(-7200, 7200), tDisplayRange(-50, 50), LevelDimension::get())
{
}

LinearBipolar50DbScaleConverter::~LinearBipolar50DbScaleConverter()
{
}
