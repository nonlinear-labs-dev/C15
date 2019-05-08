#include "DegreeDimensionCoarse.h"

DegreeDimensionCoarse::DegreeDimensionCoarse()
{
}

DegreeDimensionCoarse::~DegreeDimensionCoarse()
{
}

const DegreeDimensionCoarse &DegreeDimensionCoarse::get()
{
  static DegreeDimensionCoarse s;
  return s;
}

Glib::ustring DegreeDimensionCoarse::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, 'deg', withUnit, 0.2)";
}

Glib::ustring DegreeDimensionCoarse::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionRounded(displayValue, "deg", 0.2);
}
