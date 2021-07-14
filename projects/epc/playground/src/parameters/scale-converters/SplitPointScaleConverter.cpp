#include <parameters/scale-converters/dimension/SplitPointDimension.h>
#include "SplitPointScaleConverter.h"

SplitPointScaleConverter::SplitPointScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 60), tDisplayRange(0, 60), SplitPointDimension::get())
{
}
