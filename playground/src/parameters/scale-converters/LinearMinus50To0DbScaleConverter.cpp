#include "LinearMinus50To0DbScaleConverter.h"
#include "dimension/LevelDimension.h"

LinearMinus50To0DbScaleConverter::LinearMinus50To0DbScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 10000), tDisplayRange(-50, 0), LevelDimension::get())
{
}

LinearMinus50To0DbScaleConverter::~LinearMinus50To0DbScaleConverter()
{
}
