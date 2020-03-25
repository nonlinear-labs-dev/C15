#include <parameters/scale-converters/dimension/PitchDimensionCoarse.h>
#include "Linear60StScaleConverter.h"

Linear60StScaleConverter::Linear60StScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 6000), tDisplayRange(0, 60), PitchDimensionCoarse::get())
{
}
