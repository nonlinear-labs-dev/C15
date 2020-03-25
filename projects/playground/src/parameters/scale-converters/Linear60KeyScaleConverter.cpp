#include "Linear60KeyScaleConverter.h"
#include "dimension/KeyDimension.h"

Linear60KeyScaleConverter::Linear60KeyScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 60), tDisplayRange(0, 60), KeyDimension::get())
{
}
