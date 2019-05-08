#include "ParabolicScaleConverter.h"
#include "dimension/PercentageDimension3Digits.h"

ParabolicScaleConverter::ParabolicScaleConverter(tTcdRange tcdRange, tDisplayRange displayRange,
                                                 const Dimension &dimension)
    : ScaleConverter(dimension)
    , m_tcdRange(tcdRange)
    , m_displayRange(displayRange)
{
}

ParabolicScaleConverter::~ParabolicScaleConverter()
{
}

bool ParabolicScaleConverter::isBiPolar() const
{
  return false;
}

tTcdValue ParabolicScaleConverter::controlPositionToTcd(const tControlPositionValue &cpValue) const
{
  return m_tcdRange.scaleValueToRange(cpValue, getControlPositionRange(), false);
}

tControlPositionValue ParabolicScaleConverter::tcdToControlPosition(tTcdValue v) const
{
  return getControlPositionRange().scaleValueToRange(v, m_tcdRange, false);
}

tDisplayValue ParabolicScaleConverter::controlPositionToDisplay(const tControlPositionValue &cpValue) const
{
  tControlPositionValue in = getControlPositionRange().clip(cpValue);
  return (in * in) * m_displayRange.getRangeWidth() + m_displayRange.getMin();
}

Glib::ustring ParabolicScaleConverter::controlPositionToDisplayJS() const
{
  stringstream s;
  s << "return $wnd.formatDimension(cpValue * cpValue * " << m_displayRange.getRangeWidth() << " + "
    << m_displayRange.getMin() << ", \"" << getDimension().getStingizerJS() << "\", withUnit);";
  return s.str();
}
