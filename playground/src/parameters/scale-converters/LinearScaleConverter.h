#pragma once

#include "ScaleConverter.h"

class LinearScaleConverter : public ScaleConverter
{
  typedef ScaleConverter super;
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;

 public:
  LinearScaleConverter(const tTcdRange &tcdRange, const tDisplayRange &displayRange, const Dimension &dim);

  virtual ~LinearScaleConverter();

  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const override;

  Glib::ustring controlPositionToDisplayJS() const override;
  bool isBiPolar() const override;
  size_t hash() const override;
};
