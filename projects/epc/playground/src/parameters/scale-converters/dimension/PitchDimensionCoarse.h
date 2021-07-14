#pragma once

#include "Dimension.h"

class PitchDimensionCoarse : public Dimension
{
 private:
  PitchDimensionCoarse();

 public:
  virtual ~PitchDimensionCoarse();

  static const PitchDimensionCoarse& get();

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue& displayValue) const override;
};
