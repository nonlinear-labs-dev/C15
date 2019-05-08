#include "Linear25DbScaleConverter.h"
#include "dimension/LevelDimension.h"

Linear25DbScaleConverter::Linear25DbScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 10000), tDisplayRange(0, 25), LevelDimension::get())
{
}

Linear25DbScaleConverter::~Linear25DbScaleConverter()
{
}
