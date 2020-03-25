#include <cmath>
#include "KeyDimension.h"

Glib::ustring KeyDimension::stringize(const tDisplayValue& displayValue) const
{
  return stringizeNote(displayValue);
}

Glib::ustring KeyDimension::getStingizerJS() const
{
  return "formatNote(cpValue)";
}

Glib::ustring KeyDimension::stringizeNote(int note)
{
  const std::array<const char*, KeysPerOctave> keyLabels
      = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

  auto index = note % keyLabels.size();
  auto base = keyLabels[index];
  auto octave = static_cast<int>(note / keyLabels.size()) + 1;
  return base + std::to_string(octave);
}
KeyDimension& KeyDimension::get()
{
  static KeyDimension ret;
  return ret;
}
