#pragma once
#include "NoteDimension.h"

class NoteDimensionIgnoreOctave : public Dimension
{
 public:
  static NoteDimensionIgnoreOctave& get();
  Glib::ustring getStingizerJS() const override;
  Glib::ustring stringize(const tDisplayValue& displayValue) const override;

 private:
  NoteDimensionIgnoreOctave();
};
