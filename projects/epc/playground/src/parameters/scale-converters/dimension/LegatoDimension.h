#pragma once
#include "Dimension.h"

class LegatoDimension : public Dimension
{
 public:
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue& displayValue) const override;
  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  static const LegatoDimension& get();
};
