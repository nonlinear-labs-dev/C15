#pragma once

#include "ScaleConverter.h"

class LinearScaleConverter : public ScaleConverter
{
  typedef ScaleConverter super;
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;

 public:
  LinearScaleConverter(const tTcdRange &tcdRange, const tDisplayRange &displayRange, const Dimension &dim);

  ~LinearScaleConverter() override;

  [[nodiscard]] tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tControlPositionValue tcdToControlPosition(tTcdValue v) const override;

  [[nodiscard]] Glib::ustring controlPositionToDisplayJS() const override;
  [[nodiscard]] bool isBiPolar() const override;
  [[nodiscard]] size_t hash() const override;
};
