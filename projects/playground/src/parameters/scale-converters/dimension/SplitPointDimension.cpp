#include <cmath>
#include "SplitPointDimension.h"

Glib::ustring SplitPointDimension::stringizeSplitPointDisplay(tControlPositionValue value, VoiceGroup seenFrom)
{
  int note = std::round(value * 59);

  if(seenFrom == VoiceGroup::II)
    note++;

  return KeyDimension::stringizeNote(note);
}

Glib::ustring SplitPointDimension::stringize(const tDisplayValue &displayValue) const
{
#warning TODO
  return stringizeSplitPointDisplay(displayValue, VoiceGroup::I);
}

const SplitPointDimension &SplitPointDimension::get()
{
  static SplitPointDimension dim;
  return dim;
}
