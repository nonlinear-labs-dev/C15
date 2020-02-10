#pragma once
#include "dimension/OnOffDimension.h"
#include "LinearCountScaleConverter.h"

class OnOffScaleConverter : public LinearCountScaleConverter<2, OnOffDimension>
{
 public:
  tDisplayValue controlPositionToDisplay(const tControlPositionValue& cpValue) const override;
  tTcdValue controlPositionToTcd(const tControlPositionValue& cpValue) const override;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  bool isBiPolar() const override;
};
