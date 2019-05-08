#pragma once

#include "ScaleConverter.h"

class CombDecayBipolarMsScaleConverter : public ScaleConverter
{
 public:
  CombDecayBipolarMsScaleConverter();
  virtual ~CombDecayBipolarMsScaleConverter();

  bool isBiPolar() const override;
  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  virtual tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  Glib::ustring controlPositionToDisplayJS() const override;

 private:
  tDisplayRange m_displayRange;
  tTcdRange m_tcdRange;
};
