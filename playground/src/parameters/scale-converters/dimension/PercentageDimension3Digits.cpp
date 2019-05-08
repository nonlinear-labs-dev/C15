#include "PercentageDimension3Digits.h"

PercentageDimension3Digits::PercentageDimension3Digits()
{
}

PercentageDimension3Digits::~PercentageDimension3Digits()
{
}

const PercentageDimension3Digits &PercentageDimension3Digits::get()
{
  static PercentageDimension3Digits s;
  return s;
}

Glib::ustring PercentageDimension3Digits::getStingizerJS() const
{
  return "formatDimensionDigits(cpValue, '%', withUnit, 3)";
}

Glib::ustring PercentageDimension3Digits::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionDigits(displayValue, "%", 3);
}
