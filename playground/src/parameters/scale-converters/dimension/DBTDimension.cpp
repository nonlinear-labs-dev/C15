#include "DBTDimension.h"

DBTDimension::DBTDimension()
{
}

DBTDimension::~DBTDimension()
{
}

const DBTDimension &DBTDimension::get()
{
  static DBTDimension s;
  return s;
}

Glib::ustring DBTDimension::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, 'dB_T', withUnit, 0.1)";
}

Glib::ustring DBTDimension::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionRounded(displayValue, "dB_T", 0.1);
}
