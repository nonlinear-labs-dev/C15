#include "Linear180DegreeScaleConverter.h"
#include "dimension/DegreeDimension.h"

Linear180DegreeScaleConverter::Linear180DegreeScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 14400), tDisplayRange(0, 180), DegreeDimension::get())
{
}

Linear180DegreeScaleConverter::~Linear180DegreeScaleConverter()
{
}
