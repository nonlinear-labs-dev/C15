#pragma once

#include "playground.h"
#include "scale-converters/dimension/Dimension.h"
#include <sstream>
#include <functional>
#include <cmath>
#include <math.h>
#include <limits>

class Dimension;

template <typename tValue> class ValueRange
{
  tValue m_min;
  tValue m_max;

 public:
  explicit ValueRange(tValue min = 0, tValue max = 0)
      : m_min(min)
      , m_max(max)
  {
  }

  tValue getMin() const
  {
    return m_min;
  }

  tValue getMax() const
  {
    return m_max;
  }

  [[nodiscard]] bool isBiPolar() const
  {
    return m_min == -m_max;
  }

  template <typename tInValue>
  tValue scaleValueToRange(const tInValue &in, const ValueRange<tInValue> &inRange, bool clip) const
  {
    tInValue inRangeWidth = inRange.getRangeWidth();
    tValue outRangeWidth = getRangeWidth();
    tInValue clippedIn = clip ? inRange.clip(in) : in;
    auto res = (clippedIn - inRange.getMin()) * outRangeWidth / inRangeWidth + getMin();

    if(std::numeric_limits<tValue>::is_integer)
      return lround(res);

    return res;
  }

  template <typename tInValue>
  Glib::ustring getScaleValueToRangeJS(const ValueRange<tInValue> &inRange, const Dimension &dim) const
  {
    tInValue inRangeWidth = inRange.getRangeWidth();
    tValue outRangeWidth = getRangeWidth();
    std::stringstream s;

    double cpMin = 0;

    if(isBiPolar())
      cpMin = -1;

    s << "return $wnd.formatDimension((cpValue - " << cpMin << ") * " << outRangeWidth << " / " << inRangeWidth << " + "
      << getMin() << ", \"" << dim.getStingizerJS() << "\", withUnit);";
    return s.str();
  }

  tValue getRangeWidth() const
  {
    return m_max - m_min;
  }

  tValue clip(const tValue &in) const
  {
    if(in < m_min)
      return m_min;
    if(in > m_max)
      return m_max;
    return in;
  }

  bool matches(const tValue &in) const
  {
    return m_min <= in && m_max > in;
  }

  [[nodiscard]] size_t hash() const
  {
    std::hash<tValue> h;
    return h(m_min) ^ h(m_max);
  }
};

template <typename T> inline bool operator==(const ValueRange<T> &a, const ValueRange<T> &b)
{
  return (a.getMin() == b.getMin()) && (a.getMax() == b.getMax());
}

template <typename T> inline bool operator!=(const ValueRange<T> &a, const ValueRange<T> &b)
{
  return (a.getMin() != b.getMin()) || (a.getMax() != b.getMax());
}
