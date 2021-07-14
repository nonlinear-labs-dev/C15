#pragma once

#include "Dimension.h"

class UnitlessDimension : public Dimension
{
 public:
  UnitlessDimension();
  virtual ~UnitlessDimension();

  static const UnitlessDimension &get();

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
