#include "PercentageDimension.h"

PercentageDimension::PercentageDimension()
{
}

PercentageDimension::~PercentageDimension()
{
}

const PercentageDimension &PercentageDimension::get()
{
  static PercentageDimension s;
  return s;
}

Glib::ustring PercentageDimension::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, '%', withUnit, 0.1)";
}

Glib::ustring PercentageDimension::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionRounded(displayValue, "%", 0.1);
}
