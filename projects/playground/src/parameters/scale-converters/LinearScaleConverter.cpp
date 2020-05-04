#include "LinearScaleConverter.h"

LinearScaleConverter::LinearScaleConverter(const tTcdRange &tcdRange, const tDisplayRange &displayRange,
                                           const Dimension &dim)
    : ScaleConverter(dim)
    , m_tcdRange(tcdRange)
    , m_displayRange(displayRange)
{
}

LinearScaleConverter::~LinearScaleConverter()
{
}

bool LinearScaleConverter::isBiPolar() const
{
  return m_displayRange.isBiPolar();
}

tDisplayValue LinearScaleConverter::controlPositionToDisplay(const tControlPositionValue &cpValue) const
{
  return m_displayRange.scaleValueToRange(cpValue, getControlPositionRange(), true);
}

tTcdValue LinearScaleConverter::controlPositionToTcd(const tControlPositionValue &cpValue) const
{
  return m_tcdRange.scaleValueToRange(cpValue, getControlPositionRange(), false);
}

tControlPositionValue LinearScaleConverter::tcdToControlPosition(tTcdValue v) const
{
  return getControlPositionRange().scaleValueToRange(v, getTcdRange(), false);
}

Glib::ustring LinearScaleConverter::controlPositionToDisplayJS() const
{
  return m_displayRange.getScaleValueToRangeJS(getControlPositionRange(), getDimension());
}

size_t LinearScaleConverter::hash() const
{
  return super::hash() ^ m_tcdRange.hash() ^ (m_displayRange.hash() << 1);
}