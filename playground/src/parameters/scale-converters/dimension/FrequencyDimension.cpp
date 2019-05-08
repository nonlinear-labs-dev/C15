#include "FrequencyDimension.h"

FrequencyDimension::FrequencyDimension()
{
}

FrequencyDimension::~FrequencyDimension()
{
}

const FrequencyDimension &FrequencyDimension::get()
{
  static FrequencyDimension d;
  return d;
}

Glib::ustring FrequencyDimension::getStingizerJS() const
{
  return "formatDimensionDigits(cpValue, 'Hz', withUnit, 3)";
}

Glib::ustring FrequencyDimension::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionDigits(displayValue, "Hz", 3);
}
