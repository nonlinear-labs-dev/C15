#include "CombDecayBipolarMsScaleConverter.h"
#include "dimension/TimeDimension.h"

CombDecayBipolarMsScaleConverter::CombDecayBipolarMsScaleConverter()
    : ScaleConverter(TimeDimension<3>::get())
    , m_displayRange(1, 100000)
    , m_tcdRange(-8000, 8000)
{
}

CombDecayBipolarMsScaleConverter::~CombDecayBipolarMsScaleConverter()
{
}

bool CombDecayBipolarMsScaleConverter::isBiPolar() const
{
  return true;
}

tTcdValue CombDecayBipolarMsScaleConverter::controlPositionToTcd(const tControlPositionValue &cpValue) const
{
  if(cpValue == 0)
    return 0;

  return m_tcdRange.scaleValueToRange(cpValue, getControlPositionRange(), false);
}

tControlPositionValue CombDecayBipolarMsScaleConverter::tcdToControlPosition(tTcdValue v) const
{
  if(v == 0)
    return 0;

  return getControlPositionRange().scaleValueToRange(v, m_tcdRange, false);
}

tDisplayValue CombDecayBipolarMsScaleConverter::controlPositionToDisplay(const tControlPositionValue &cpValue) const
{
  auto log_min = log(m_displayRange.getMin());
  auto log_max = log(m_displayRange.getMax());
  auto logDiff = (log_max - log_min);
  tControlPositionValue clippedValue = getControlPositionRange().clip(cpValue);

  if(clippedValue == 0.0)
    return 0.0;

  tDisplayValue ret = exp(log_min + fabs(clippedValue) * logDiff);
  return clippedValue < 0 ? -ret : ret;
}

Glib::ustring CombDecayBipolarMsScaleConverter::controlPositionToDisplayJS() const
{
  auto log_min = log(m_displayRange.getMin());
  auto log_max = log(m_displayRange.getMax());
  auto logDiff = (log_max - log_min);

  Glib::ustring dimensionStringizer = getDimension().getStingizerJS();
  std::stringstream s;

  s << "return $wnd.formatDimension((cpValue < 0 ? -1 : 1) * (cpValue == 0 ? 0 : Math.exp(" << log_min
    << " + Math.abs(cpValue) * " << logDiff << ")), \"" << dimensionStringizer << "\", withUnit);";

  Glib::ustring ret = s.str();
  return ret;
}