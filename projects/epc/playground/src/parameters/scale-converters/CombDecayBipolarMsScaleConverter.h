#pragma once

#include "ScaleConverter.h"

class CombDecayBipolarMsScaleConverter : public ScaleConverter
{
 public:
  CombDecayBipolarMsScaleConverter();
  ~CombDecayBipolarMsScaleConverter() override;

  [[nodiscard]] bool isBiPolar() const override;
  [[nodiscard]] tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  [[nodiscard]] Glib::ustring controlPositionToDisplayJS() const override;

 private:
  tDisplayRange m_displayRange;
  tTcdRange m_tcdRange;
};
