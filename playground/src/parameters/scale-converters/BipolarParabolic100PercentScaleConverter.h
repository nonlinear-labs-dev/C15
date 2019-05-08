#pragma once

#include "ParabolicScaleConverter.h"

class BipolarParabolic100PercentScaleConverter : public ScaleConverter
{
 public:
  BipolarParabolic100PercentScaleConverter();
  virtual ~BipolarParabolic100PercentScaleConverter();

  bool isBiPolar() const;
  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const;
  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const;
  Glib::ustring controlPositionToDisplayJS() const;

 private:
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;
};
