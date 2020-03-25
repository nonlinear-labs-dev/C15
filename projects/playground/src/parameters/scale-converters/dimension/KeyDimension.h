#pragma once
#include "Dimension.h"

class KeyDimension : public Dimension
{
 public:
  Glib::ustring stringize(const tDisplayValue& displayValue) const override;
  static Glib::ustring stringizeNote(int note);
  Glib::ustring getStingizerJS() const override;

  static KeyDimension& get();

 private:
  static constexpr auto OctaveCount = 5;
  static constexpr auto KeysPerOctave = 12;
};
