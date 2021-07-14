#pragma once

#include "Dimension.h"

class PitchDimensionFine : public Dimension
{
 private:
  PitchDimensionFine();

 public:
  virtual ~PitchDimensionFine();

  static const PitchDimensionFine& get();
  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue& displayValue) const override;
};
