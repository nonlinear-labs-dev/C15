#include "EditSmoothingTimeMSScaleConverter.h"
#include "dimension/TimeDimension.h"

EditSmoothingTimeMSScaleConverter::EditSmoothingTimeMSScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 16000), tDisplayRange(0, 200.0), TimeDimension::get())
{
}

EditSmoothingTimeMSScaleConverter::~EditSmoothingTimeMSScaleConverter()
{
}
