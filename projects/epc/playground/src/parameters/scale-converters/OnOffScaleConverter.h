#pragma once
#include "dimension/OnOffDimension.h"
#include "LinearCountScaleConverter.h"

class OnOffScaleConverter : public LinearCountScaleConverter<2, OnOffDimension>
{
 public:
  [[nodiscard]] tDisplayValue controlPositionToDisplay(const tControlPositionValue& cpValue) const override;
  [[nodiscard]] tTcdValue controlPositionToTcd(const tControlPositionValue& cpValue) const override;
  [[nodiscard]] tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  [[nodiscard]] bool isBiPolar() const override;
};
