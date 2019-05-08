#include "DegreeDimension.h"

DegreeDimension::DegreeDimension()
{
}

DegreeDimension::~DegreeDimension()
{
}

const DegreeDimension &DegreeDimension::get()
{
  static DegreeDimension s;
  return s;
}

Glib::ustring DegreeDimension::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, 'deg', withUnit, 0.1)";
}

Glib::ustring DegreeDimension::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionRounded(displayValue, "deg", 0.1);
}
