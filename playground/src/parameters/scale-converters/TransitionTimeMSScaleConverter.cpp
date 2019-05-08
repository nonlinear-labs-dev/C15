#include "TransitionTimeMSScaleConverter.h"

TransitionTimeMSScaleConverter::TransitionTimeMSScaleConverter()
    : TimeScaleConverter(tTcdRange(0, 16000), tDisplayRange(0.1, 16000.0))
{
}

TransitionTimeMSScaleConverter::~TransitionTimeMSScaleConverter()
{
}
