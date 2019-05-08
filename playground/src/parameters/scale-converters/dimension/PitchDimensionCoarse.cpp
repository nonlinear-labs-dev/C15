#include "PitchDimensionCoarse.h"

PitchDimensionCoarse::PitchDimensionCoarse()
{
}

PitchDimensionCoarse::~PitchDimensionCoarse()
{
}

Glib::ustring PitchDimensionCoarse::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, 'st', withUnit, 0.1)";
}

const PitchDimensionCoarse& PitchDimensionCoarse::get()
{
  static PitchDimensionCoarse p;
  return p;
}

Glib::ustring PitchDimensionCoarse::stringize(const tDisplayValue& displayValue) const
{
  return formatDimensionRounded(displayValue, "st", 0.1);
}
