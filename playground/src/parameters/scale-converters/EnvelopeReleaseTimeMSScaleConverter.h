#pragma once

#include <parameters/scale-converters/ScaleConverter.h>
#include <playground.h>

class Dimension;
class QuantizedValue;

class EnvelopeReleaseTimeMSScaleConverter : public ScaleConverter
{
 private:
 public:
  EnvelopeReleaseTimeMSScaleConverter();
  virtual ~EnvelopeReleaseTimeMSScaleConverter();

  EnvelopeReleaseTimeMSScaleConverter(const EnvelopeReleaseTimeMSScaleConverter &) = delete;
  EnvelopeReleaseTimeMSScaleConverter &operator=(const EnvelopeReleaseTimeMSScaleConverter &) = delete;

  tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  Glib::ustring controlPositionToDisplayJS() const override;
  bool isBiPolar() const override;
};
