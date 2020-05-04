#include "ParabolicGainDbScaleConverter.h"
#include "dimension/LevelDimension3Digits.h"

ParabolicGainDbScaleConverter::ParabolicGainDbScaleConverter()
    : ScaleConverter(LevelDimension3Digits::get())
{
}

ParabolicGainDbScaleConverter::~ParabolicGainDbScaleConverter()
{
}

bool ParabolicGainDbScaleConverter::isBiPolar() const
{
  return false;
}

tDisplayValue ParabolicGainDbScaleConverter::controlPositionToDisplay(const tControlPositionValue &cpValue) const
{
  tControlPositionValue in = getControlPositionRange().clip(cpValue);

  if(in == 0)
    return -128;

  return 20 * log10(4 * in * in);
}

tControlPositionValue ParabolicGainDbScaleConverter::displayToControlPosition(const tDisplayValue &displayValue) const
{
  if(displayValue == -128)
    return 0;

  auto cp = std::sqrt(std::pow(10, (displayValue / 20)) / 4);
  return getControlPositionRange().clip(cp);
}

tTcdValue ParabolicGainDbScaleConverter::controlPositionToTcd(const tControlPositionValue &cpValue) const
{
  return 16000 * cpValue;
}

tControlPositionValue ParabolicGainDbScaleConverter::tcdToControlPosition(tTcdValue v) const
{
  return v / 16000.0;
}

Glib::ustring ParabolicGainDbScaleConverter::controlPositionToDisplayJS() const
{
  std::stringstream s;
  s << "return $wnd.formatDimension(cpValue == 0 ? -128 : (20 * Math.log (4 * cpValue * cpValue) / Math.log(10)), \""
    << getDimension().getStingizerJS() << "\", withUnit);";
  return s.str();
}