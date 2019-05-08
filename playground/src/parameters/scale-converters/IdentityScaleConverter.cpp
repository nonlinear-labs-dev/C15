#include "IdentityScaleConverter.h"
#include "dimension/UnitlessDimension.h"

IdentityScaleConverter::IdentityScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 16000), tDisplayRange(0, 100), UnitlessDimension::get())
{
}

IdentityScaleConverter::~IdentityScaleConverter()
{
}
