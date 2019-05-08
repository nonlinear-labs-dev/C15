#include "LevelScaleConverter.h"
#include "dimension/LevelDimension.h"

LevelScaleConverter::LevelScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 1 << 13), tDisplayRange(0, 100), LevelDimension::get())
{
}

LevelScaleConverter::~LevelScaleConverter()
{
}
