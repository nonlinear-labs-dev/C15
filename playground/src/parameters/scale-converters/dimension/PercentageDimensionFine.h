#pragma once

#include "Dimension.h"

class PercentageDimensionFine : public Dimension
{
 public:
  virtual ~PercentageDimensionFine();

  Glib::ustring getStingizerJS() const override;
  static const PercentageDimensionFine &get();
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  PercentageDimensionFine();
};
