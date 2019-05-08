#pragma once

#include "ScaleConverter.h"

class ParabolicScaleConverter : public ScaleConverter
{
 public:
  ParabolicScaleConverter(tTcdRange tcdRange, tDisplayRange displayRange, const Dimension &dimension);
  virtual ~ParabolicScaleConverter();

  bool isBiPolar() const override;
  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  Glib::ustring controlPositionToDisplayJS() const override;

  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const override;

 private:
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;
};
