#include <parameters/scale-converters/Parabolic2000MsScaleConverter.h>
#include "dimension/TimeDimension.h"

Parabolic2000MsScaleConverter::Parabolic2000MsScaleConverter()
    : ParabolicScaleConverter(tTcdRange(0, 16000), tDisplayRange(0, 2000), TimeDimension::get())
{
}

Parabolic2000MsScaleConverter::~Parabolic2000MsScaleConverter()
{
}
