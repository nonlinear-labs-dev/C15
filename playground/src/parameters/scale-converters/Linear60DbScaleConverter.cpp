#include "Linear60DbScaleConverter.h"
#include "dimension/LevelDimension.h"

Linear60DbScaleConverter::Linear60DbScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 15360), tDisplayRange(0, 60), LevelDimension::get())
{
}

Linear60DbScaleConverter::~Linear60DbScaleConverter()
{
}
