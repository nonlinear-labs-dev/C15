#include "BaseKeyScaleConverter.h"
#include "dimension/NoteDimension.h"

BaseKeyScaleConverter::BaseKeyScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 11), tDisplayRange(0, 11), NoteDimension::get())
{
}

BaseKeyScaleConverter::~BaseKeyScaleConverter()
{
}
