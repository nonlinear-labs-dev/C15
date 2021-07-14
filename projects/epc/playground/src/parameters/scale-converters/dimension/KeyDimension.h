#pragma once
#include "Dimension.h"

class KeyDimension : public Dimension
{
 public:
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue& displayValue) const override;
  [[nodiscard]] virtual Glib::ustring stringizeNote(int note) const;
  [[nodiscard]] Glib::ustring getStingizerJS() const override;

  static KeyDimension& get();

 private:
  static constexpr auto OctaveCount = 5;
  static constexpr auto KeysPerOctave = 12;
};
