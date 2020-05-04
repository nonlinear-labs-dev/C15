#include <parameters/scale-converters/BipolarParabolic100PercentScaleConverter.h>
#include "dimension/PercentageDimension3Digits.h"

BipolarParabolic100PercentScaleConverter::BipolarParabolic100PercentScaleConverter()
    : ScaleConverter(PercentageDimension3Digits::get())
    , m_tcdRange(-8000, 8000)
    , m_displayRange(-100, 100)
{
}

BipolarParabolic100PercentScaleConverter::~BipolarParabolic100PercentScaleConverter()
{
}

bool BipolarParabolic100PercentScaleConverter::isBiPolar() const
{
  return true;
}

tTcdValue BipolarParabolic100PercentScaleConverter::controlPositionToTcd(const tControlPositionValue &cpValue) const
{
  return m_tcdRange.scaleValueToRange(cpValue, getControlPositionRange(), false);
}

tControlPositionValue BipolarParabolic100PercentScaleConverter::tcdToControlPosition(tTcdValue v) const
{
  return getControlPositionRange().scaleValueToRange(v, m_tcdRange, false);
}

tDisplayValue
    BipolarParabolic100PercentScaleConverter::controlPositionToDisplay(const tControlPositionValue &cpValue) const
{
  tControlPositionValue in = getControlPositionRange().clip(cpValue);
  return (in * std::abs(in)) * 100;
}

Glib::ustring BipolarParabolic100PercentScaleConverter::controlPositionToDisplayJS() const
{
  std::stringstream s;
  s << "return $wnd.formatDimension(cpValue * Math.abs(cpValue) * 100, \"" << getDimension().getStingizerJS()
    << "\", withUnit);";
  return s.str();
}