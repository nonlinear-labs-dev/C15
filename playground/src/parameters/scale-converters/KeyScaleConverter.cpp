#include "KeyScaleConverter.h"
#include "dimension/CentDimension.h"

static const double c_range = 800.0;

KeyScaleConverter::KeyScaleConverter()
    : LinearScaleConverter(tTcdRange(-10 * c_range, 10 * c_range), tDisplayRange(-c_range, c_range),
                           CentDimension::get())
{
}

KeyScaleConverter::~KeyScaleConverter()
{
}
