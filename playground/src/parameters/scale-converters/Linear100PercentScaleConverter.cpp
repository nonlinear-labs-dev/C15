#include "Linear100PercentScaleConverter.h"
#include "dimension/PercentageDimension.h"

Linear100PercentScaleConverter::Linear100PercentScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 16000), tDisplayRange(0, 100), PercentageDimension::get())
{
}

Linear100PercentScaleConverter::~Linear100PercentScaleConverter()
{
}
