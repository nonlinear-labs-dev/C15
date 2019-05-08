#include "MacroControlScaleConverter.h"
#include "dimension/PercentageDimension.h"

MacroControlScaleConverter::MacroControlScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 3200), tDisplayRange(0, 100), PercentageDimension::get())
{
}
