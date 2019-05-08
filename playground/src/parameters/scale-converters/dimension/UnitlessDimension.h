#pragma once

#include "Dimension.h"

class UnitlessDimension : public Dimension
{
 public:
  UnitlessDimension();
  virtual ~UnitlessDimension();

  static const UnitlessDimension &get();
  Glib::ustring getStingizerJS() const override;
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
