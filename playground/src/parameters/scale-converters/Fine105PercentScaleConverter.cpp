#include "Fine105PercentScaleConverter.h"
#include "dimension/PercentageDimensionFine.h"

Fine105PercentScaleConverter::Fine105PercentScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 10500), tDisplayRange(0, 105), PercentageDimensionFine::get())
{
}

Fine105PercentScaleConverter::~Fine105PercentScaleConverter()
{
}
