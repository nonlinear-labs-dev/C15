#pragma once

#include "Dimension.h"

class PercentageDimension3Digits : public Dimension
{
 public:
  virtual ~PercentageDimension3Digits();

  Glib::ustring getStingizerJS() const override;
  static const PercentageDimension3Digits &get();
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  PercentageDimension3Digits();
};
