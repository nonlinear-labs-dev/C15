#include "OnOffScaleConverter.h"

tDisplayValue OnOffScaleConverter::controlPositionToDisplay(const tControlPositionValue& cpValue) const
{
  return cpValue;
}

tTcdValue OnOffScaleConverter::controlPositionToTcd(const tControlPositionValue& cpValue) const
{
  return cpValue;
}

tControlPositionValue OnOffScaleConverter::tcdToControlPosition(tTcdValue v) const
{
  return v;
}

bool OnOffScaleConverter::isBiPolar() const
{
  return false;
}
