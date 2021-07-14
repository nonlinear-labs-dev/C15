#pragma once

#include "Dimension.h"

class PercentageDimension : public Dimension
{
 public:
  virtual ~PercentageDimension();

  static const PercentageDimension &get();

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  PercentageDimension();
};
