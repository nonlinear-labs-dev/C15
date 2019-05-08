#include "PitchDimension.h"

PitchDimension::PitchDimension()
{
}

PitchDimension::~PitchDimension()
{
}

Glib::ustring PitchDimension::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, 'st', withUnit, 0.01)";
}

const PitchDimension& PitchDimension::get()
{
  static PitchDimension p;
  return p;
}

Glib::ustring PitchDimension::stringize(const tDisplayValue& displayValue) const
{
  return formatDimensionRounded(displayValue, "st", 0.01);
}
