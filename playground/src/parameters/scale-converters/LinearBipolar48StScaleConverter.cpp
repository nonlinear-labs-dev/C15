#include "LinearBipolar48StScaleConverter.h"
#include "dimension/PitchDimension.h"

LinearBipolar48StScaleConverter::LinearBipolar48StScaleConverter()
    : LinearScaleConverter(tTcdRange(-4800, 4800), tDisplayRange(-48, 48), PitchDimension::get())
{
}

LinearBipolar48StScaleConverter::~LinearBipolar48StScaleConverter()
{
}
