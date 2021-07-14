#pragma once

#include "Dimension.h"

class PercentageDimensionFine : public Dimension
{
 public:
  virtual ~PercentageDimensionFine();

  static const PercentageDimensionFine &get();

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  PercentageDimensionFine();
};
