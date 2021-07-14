#pragma once
#include "NoteDimension.h"

class NoteDimensionIgnoreOctave : public Dimension
{
 public:
  static NoteDimensionIgnoreOctave& get();
  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue& displayValue) const override;

 private:
  NoteDimensionIgnoreOctave();
};
