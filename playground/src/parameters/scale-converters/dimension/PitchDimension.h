#pragma once

#include "Dimension.h"

class PitchDimension : public Dimension
{
 private:
  PitchDimension();

 public:
  virtual ~PitchDimension();

  static const PitchDimension& get();
  Glib::ustring getStingizerJS() const override;
  Glib::ustring stringize(const tDisplayValue& displayValue) const override;
};
