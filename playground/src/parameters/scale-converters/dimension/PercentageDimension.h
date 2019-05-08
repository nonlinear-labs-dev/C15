#pragma once

#include "Dimension.h"

class PercentageDimension : public Dimension
{
 public:
  virtual ~PercentageDimension();

  Glib::ustring getStingizerJS() const override;
  static const PercentageDimension &get();
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  PercentageDimension();
};
