#pragma once

#include "Dimension.h"

class DegreeDimension : public Dimension
{
 private:
  DegreeDimension();

 public:
  virtual ~DegreeDimension();

  static const DegreeDimension &get();
  Glib::ustring getStingizerJS() const override;

  Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
