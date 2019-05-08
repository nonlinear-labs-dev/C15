#pragma once

#include "ScaleConverter.h"

class TimeScaleConverter : public ScaleConverter
{
  typedef ScaleConverter super;

 public:
  TimeScaleConverter(const tTcdRange &tcdRange, const tDisplayRange &displayRange);
  virtual ~TimeScaleConverter();

  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  Glib::ustring controlPositionToDisplayJS() const override;
  bool isBiPolar() const override;
  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const override;

  size_t hash() const override;

 private:
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;
};
