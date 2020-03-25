#pragma once
#include "Dimension.h"

class LegatoDimension : public Dimension
{
 public:
  Glib::ustring stringize(const tDisplayValue& displayValue) const override;
  Glib::ustring getStingizerJS() const override;
  static const LegatoDimension& get();
};
