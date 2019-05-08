#include "EnvelopeTimeMSScaleConverter.h"

EnvelopeTimeMSScaleConverter::EnvelopeTimeMSScaleConverter()
    : TimeScaleConverter(tTcdRange(0, 16000), tDisplayRange(1.0, 160000.0))
{
}

EnvelopeTimeMSScaleConverter::~EnvelopeTimeMSScaleConverter()
{
}
