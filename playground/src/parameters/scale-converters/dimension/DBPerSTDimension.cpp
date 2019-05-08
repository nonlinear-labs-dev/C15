#include "DBPerSTDimension.h"

DBPerSTDimension::DBPerSTDimension()
{
}

DBPerSTDimension::~DBPerSTDimension()
{
}

const DBPerSTDimension &DBPerSTDimension::get()
{
  static DBPerSTDimension s;
  return s;
}

Glib::ustring DBPerSTDimension::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, 'dB/st', withUnit, 0.001)";
}

Glib::ustring DBPerSTDimension::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionRounded(displayValue, "dB/st", 0.001);
}
