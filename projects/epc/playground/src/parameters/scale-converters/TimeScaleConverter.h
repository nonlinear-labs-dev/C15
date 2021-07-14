#pragma once

#include "ScaleConverter.h"

class TimeScaleConverter : public ScaleConverter
{
  typedef ScaleConverter super;

 public:
  TimeScaleConverter(const tTcdRange &tcdRange, const tDisplayRange &displayRange);
  ~TimeScaleConverter() override;

  [[nodiscard]] tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] Glib::ustring controlPositionToDisplayJS() const override;
  [[nodiscard]] bool isBiPolar() const override;
  [[nodiscard]] tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tControlPositionValue tcdToControlPosition(tTcdValue v) const override;

  [[nodiscard]] size_t hash() const override;

 private:
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;
};
