#pragma once

#include "Dimension.h"

class PercentageDimension3Digits : public Dimension
{
 public:
  virtual ~PercentageDimension3Digits();

  static const PercentageDimension3Digits &get();

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  PercentageDimension3Digits();
};
