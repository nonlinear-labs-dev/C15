#include <parameters/scale-converters/Parabolic50MsScaleConverter.h>
#include "dimension/TimeDimension.h"

Parabolic50MsScaleConverter::Parabolic50MsScaleConverter()
    : ParabolicScaleConverter(tTcdRange(0, 12500), tDisplayRange(0, 50), TimeDimension::get())
{
}

Parabolic50MsScaleConverter::~Parabolic50MsScaleConverter()
{
}
