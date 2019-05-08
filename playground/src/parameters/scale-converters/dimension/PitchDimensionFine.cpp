#include "PitchDimensionFine.h"

PitchDimensionFine::PitchDimensionFine()
{
}

PitchDimensionFine::~PitchDimensionFine()
{
}

Glib::ustring PitchDimensionFine::getStingizerJS() const
{
  return "formatDimensionRounded(cpValue, 'st', withUnit, 0.001)";
}

const PitchDimensionFine& PitchDimensionFine::get()
{
  static PitchDimensionFine p;
  return p;
}

Glib::ustring PitchDimensionFine::stringize(const tDisplayValue& displayValue) const
{
  return formatDimensionRounded(displayValue, "st", 0.001);
}
