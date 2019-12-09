#include "TimeScaleConverter.h"
#include "dimension/TimeDimension.h"
#include <math.h>

TimeScaleConverter::TimeScaleConverter(const tTcdRange &tcdRange, const tDisplayRange &displayRange)
    : ScaleConverter(TimeDimension::get())
    , m_tcdRange(tcdRange)
    , m_displayRange(displayRange)
{
}

TimeScaleConverter::~TimeScaleConverter()
{
}

bool TimeScaleConverter::isBiPolar() const
{
  return false;
}

tTcdValue TimeScaleConverter::controlPositionToTcd(const tControlPositionValue &cpValue) const
{
  return m_tcdRange.scaleValueToRange(cpValue, getControlPositionRange(), false);
}

tControlPositionValue TimeScaleConverter::tcdToControlPosition(tTcdValue v) const
{
  return getControlPositionRange().scaleValueToRange(v, getTcdRange(), false);
}

tDisplayValue TimeScaleConverter::controlPositionToDisplay(const tControlPositionValue &cpValue) const
{
  auto log_min = m_displayRange.getMin() == 0 ? 0.0 : log(m_displayRange.getMin());
  auto log_max = log(m_displayRange.getMax());
  auto logDiff = (log_max - log_min);
  auto ret = exp(log_min + getControlPositionRange().clip(cpValue) * logDiff);

  if(m_displayRange.getMin() == 0.0)
  {
    ret -= 1.0;
    ret *= m_displayRange.getMax() / (m_displayRange.getMax() - 1);
  }

  return ret;
}

Glib::ustring TimeScaleConverter::controlPositionToDisplayJS() const
{
  auto log_min = m_displayRange.getMin() == 0 ? 0.0 : log(m_displayRange.getMin());
  auto log_max = log(m_displayRange.getMax());
  auto logDiff = (log_max - log_min);
  std::stringstream s;
  s << "var e = Math.exp(" << log_min << " + cpValue * " << logDiff << ");";

  if(m_displayRange.getMin() == 0.0)
  {
    auto mul = m_displayRange.getMax() / (m_displayRange.getMax() - 1);
    s << "e -= 1.0;";
    s << "e *= " << mul << ";";
  }

  s << "return $wnd.formatDimension(e, \"" << getDimension().getStingizerJS() << "\", withUnit);";
  return s.str();
}

size_t TimeScaleConverter::hash() const
{
  return super::hash() ^ m_tcdRange.hash() ^ m_displayRange.hash();
}
