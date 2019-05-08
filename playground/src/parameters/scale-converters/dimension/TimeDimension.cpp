#include "TimeDimension.h"

TimeDimension::TimeDimension()
{
}

TimeDimension::~TimeDimension()
{
}

const TimeDimension &TimeDimension::get()
{
  static TimeDimension d;
  return d;
}

Glib::ustring TimeDimension::getStingizerJS() const
{
  return "formatDimensionDigits(cpValue, 'ms', withUnit, 3)";
}

Glib::ustring TimeDimension::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionDigits(displayValue, "ms", 3);
}
