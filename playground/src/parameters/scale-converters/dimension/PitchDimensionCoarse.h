#pragma once

#include "Dimension.h"

class PitchDimensionCoarse : public Dimension
{
 private:
  PitchDimensionCoarse();

 public:
  virtual ~PitchDimensionCoarse();

  static const PitchDimensionCoarse& get();
  Glib::ustring getStingizerJS() const override;
  Glib::ustring stringize(const tDisplayValue& displayValue) const override;
};
