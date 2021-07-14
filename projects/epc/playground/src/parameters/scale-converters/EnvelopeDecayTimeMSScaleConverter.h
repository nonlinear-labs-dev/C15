#pragma once

#include "ScaleConverter.h"

class EnvelopeDecayTimeMSScaleConverter : public ScaleConverter
{
  typedef ScaleConverter super;

 public:
  EnvelopeDecayTimeMSScaleConverter();
  ~EnvelopeDecayTimeMSScaleConverter() override;

  [[nodiscard]] tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  [[nodiscard]] Glib::ustring controlPositionToDisplayJS() const override;
  [[nodiscard]] bool isBiPolar() const override;

 private:
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;
};
