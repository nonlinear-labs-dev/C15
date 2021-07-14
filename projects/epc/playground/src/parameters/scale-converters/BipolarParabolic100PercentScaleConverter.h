#pragma once

#include "ParabolicScaleConverter.h"

class BipolarParabolic100PercentScaleConverter : public ScaleConverter
{
 public:
  BipolarParabolic100PercentScaleConverter();
  ~BipolarParabolic100PercentScaleConverter() override;

  [[nodiscard]] bool isBiPolar() const override;
  [[nodiscard]] tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  [[nodiscard]] tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] Glib::ustring controlPositionToDisplayJS() const override;

 private:
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;
};
