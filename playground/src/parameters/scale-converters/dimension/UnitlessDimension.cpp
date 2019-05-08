#include "UnitlessDimension.h"

UnitlessDimension::UnitlessDimension()
{
}

UnitlessDimension::~UnitlessDimension()
{
}

const UnitlessDimension &UnitlessDimension::get()
{
  static UnitlessDimension d;
  return d;
}

Glib::ustring UnitlessDimension::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, ' ', withUnit, 0.1)";
}

Glib::ustring UnitlessDimension::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionRounded(displayValue, "", 0.1);
}
