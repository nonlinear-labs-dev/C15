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
  ~EnvelopeReleaseTimeMSScaleConverter() override;

  EnvelopeReleaseTimeMSScaleConverter(const EnvelopeReleaseTimeMSScaleConverter &) = delete;
  EnvelopeReleaseTimeMSScaleConverter &operator=(const EnvelopeReleaseTimeMSScaleConverter &) = delete;

  [[nodiscard]] tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const override;
  [[nodiscard]] tControlPositionValue tcdToControlPosition(tTcdValue v) const override;
  [[nodiscard]] Glib::ustring controlPositionToDisplayJS() const override;
  [[nodiscard]] bool isBiPolar() const override;
};
