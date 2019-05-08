#include "Parabolic10HzScaleConverter.h"
#include "dimension/FrequencyDimension.h"

Parabolic10HzScaleConverter::Parabolic10HzScaleConverter()
    : ParabolicScaleConverter(tTcdRange(0, 16000), tDisplayRange(0, 10), FrequencyDimension::get())
{
}

Parabolic10HzScaleConverter::~Parabolic10HzScaleConverter()
{
}
