#pragma once

#include "ScaleConverter.h"

class ParabolicScaleConverter : public ScaleConverter
{
 public:
  ParabolicScaleConverter(tTcdRange tcdRange, tDisplayRange displayRange, const Dimension &dimension);
  ~ParabolicScaleConverter() override;

  [[nodiscard]] bool isBiPolar() const override;
  [[nodiscard]] tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] Glib::ustring controlPositionToDisplayJS() const override;

  [[nodiscard]] tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tControlPositionValue tcdToControlPosition(tTcdValue v) const override;

 private:
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;
};
