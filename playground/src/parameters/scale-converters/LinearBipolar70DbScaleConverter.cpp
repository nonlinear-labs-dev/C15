#include <parameters/scale-converters/dimension/LevelDimension.h>
#include "LinearBipolar70DbScaleConverter.h"

LinearBipolar70DbScaleConverter::LinearBipolar70DbScaleConverter()
    : LinearScaleConverter(tTcdRange(-7200, 7200), tDisplayRange(-70, 70), LevelDimension::get())
{
}
