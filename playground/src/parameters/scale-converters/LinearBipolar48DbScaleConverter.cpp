#include "dimension/LevelDimension.h"
#include "LinearBipolar48DbScaleConverter.h"

LinearBipolar48DbScaleConverter::LinearBipolar48DbScaleConverter()
    : LinearScaleConverter(tTcdRange(-7200, 7200), tDisplayRange(-48, 48), LevelDimension::get())
{
}

LinearBipolar48DbScaleConverter::~LinearBipolar48DbScaleConverter()
{
}
