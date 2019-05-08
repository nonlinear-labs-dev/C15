#include "Linear50DbScaleConverter.h"
#include "dimension/LevelDimension.h"

Linear50DbScaleConverter::Linear50DbScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 10000), tDisplayRange(0, 50), LevelDimension::get())
{
}

Linear50DbScaleConverter::~Linear50DbScaleConverter()
{
}
