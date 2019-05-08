#include "ScaleConverter.h"
#include <parameters/value/QuantizedValue.h>

ScaleConverter::ScaleConverter(const Dimension &dim)
    : m_dimension(dim)
{
}

ScaleConverter::~ScaleConverter()
{
}

const Dimension &ScaleConverter::getDimension() const
{
  return m_dimension;
}

size_t ScaleConverter::hash() const
{
  return m_dimension.hash() ^ typeid(*this).hash_code();
}

const ScaleConverter::tControlPositionRange &ScaleConverter::getControlPositionRangeUnipolar()
{
  static tControlPositionRange s(0, 1);
  return s;
}

const ScaleConverter::tControlPositionRange &ScaleConverter::getControlPositionRangeBipolar()
{
  static tControlPositionRange s(-1, 1);
  return s;
}

ScaleConverter::tTcdRange ScaleConverter::getTcdRange() const
{
  ScaleConverter::tControlPositionRange cpRange = getControlPositionRange();
  return tTcdRange(controlPositionToTcd(cpRange.getMin()), controlPositionToTcd(cpRange.getMax()));
}

tControlPositionValue ScaleConverter::getCoarseDenominator(const QuantizedValue &v) const
{
  return v.getCoarseDenominator();
}

tControlPositionValue ScaleConverter::getFineDenominator(const QuantizedValue &v) const
{
  return v.getFineDenominator();
}

const ScaleConverter::tControlPositionRange ScaleConverter::getControlPositionRange() const
{
  return isBiPolar() ? getControlPositionRangeBipolar() : getControlPositionRangeUnipolar();
}
