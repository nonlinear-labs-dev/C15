#include <parameters/scale-converters/dimension/NoteDimension.h>
#include "BaseKeyScaleConverter.h"
#include "dimension/NoteDimensionIgnoreOctave.h"

BaseKeyScaleConverter::BaseKeyScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 11), tDisplayRange(0, 11), NoteDimensionIgnoreOctave::get())
{
}

BaseKeyScaleConverter::~BaseKeyScaleConverter()
{
}
