#include "Linear12CountScaleConverter.h"
#include "dimension/VoicesDimension.h"

Linear12CountScaleConverter::Linear12CountScaleConverter()
    : LinearScaleConverter(tTcdRange(1, 12), tDisplayRange(1, 12), VoicesDimension::get())
{
}

Linear12CountScaleConverter::~Linear12CountScaleConverter()
{
}
