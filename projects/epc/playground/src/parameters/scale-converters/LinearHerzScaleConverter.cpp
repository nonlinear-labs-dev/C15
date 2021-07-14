#include <parameters/scale-converters/dimension/FrequencyDimension.h>
#include "LinearHerzScaleConverter.h"

LinearHerzScaleConverter::LinearHerzScaleConverter()
    : LinearScaleConverter(ScaleConverter::tTcdRange { 0, 16000 }, tDisplayRange { 400, 480 },
                           FrequencyDimension::get())
{
}
