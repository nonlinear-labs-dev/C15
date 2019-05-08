#pragma once

#include "ScaleConverter.h"

class EnvelopeAttackDecayTimeMSScaleConverter : public ScaleConverter
{
  typedef ScaleConverter super;

 public:
  EnvelopeAttackDecayTimeMSScaleConverter();
  virtual ~EnvelopeAttackDecayTimeMSScaleConverter();

  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  Glib::ustring controlPositionToDisplayJS() const override;
  bool isBiPolar() const override;

 private:
  tTcdRange m_tcdRange;
};
