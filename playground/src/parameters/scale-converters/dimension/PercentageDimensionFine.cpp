#include "PercentageDimensionFine.h"

PercentageDimensionFine::PercentageDimensionFine()
{
}

PercentageDimensionFine::~PercentageDimensionFine()
{
}

const PercentageDimensionFine &PercentageDimensionFine::get()
{
  static PercentageDimensionFine s;
  return s;
}

Glib::ustring PercentageDimensionFine::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, '%', withUnit, 0.01)";
}

Glib::ustring PercentageDimensionFine::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionRounded(displayValue, "%", 0.01);
}
