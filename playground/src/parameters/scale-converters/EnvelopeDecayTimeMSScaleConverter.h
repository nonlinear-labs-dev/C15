#pragma once

#include "ScaleConverter.h"

class EnvelopeDecayTimeMSScaleConverter : public ScaleConverter
{
  typedef ScaleConverter super;

 public:
  EnvelopeDecayTimeMSScaleConverter();
  virtual ~EnvelopeDecayTimeMSScaleConverter();

  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  Glib::ustring controlPositionToDisplayJS() const override;
  bool isBiPolar() const override;

 private:
  tTcdRange m_tcdRange;
  tDisplayRange m_displayRange;
};
