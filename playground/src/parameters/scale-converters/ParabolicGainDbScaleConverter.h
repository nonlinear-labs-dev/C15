#pragma once

#include "ScaleConverter.h"

class ParabolicGainDbScaleConverter : public ScaleConverter
{
 public:
  ParabolicGainDbScaleConverter();
  virtual ~ParabolicGainDbScaleConverter();

  bool isBiPolar() const override;
  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  Glib::ustring controlPositionToDisplayJS() const override;
};
