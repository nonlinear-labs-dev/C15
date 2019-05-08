#include "CentDimension.h"

CentDimension::CentDimension()
{
}

CentDimension::~CentDimension()
{
}

const CentDimension &CentDimension::get()
{
  static CentDimension s;
  return s;
}

Glib::ustring CentDimension::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, 'ct', withUnit, 0.1)";
}

Glib::ustring CentDimension::stringize(const tDisplayValue &displayValue) const
{
  return formatDimensionRounded(displayValue, "ct", 0.1);
}
