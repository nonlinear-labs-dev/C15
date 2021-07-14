#pragma once

#include "Dimension.h"

class DegreeDimension : public Dimension
{
 private:
  DegreeDimension();

 public:
  virtual ~DegreeDimension();

  static const DegreeDimension &get();
  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
