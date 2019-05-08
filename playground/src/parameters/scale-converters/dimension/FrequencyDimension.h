#pragma once

#include "Dimension.h"

class FrequencyDimension : public Dimension
{
 public:
  FrequencyDimension();
  virtual ~FrequencyDimension();

  static const FrequencyDimension &get();
  Glib::ustring getStingizerJS() const override;

  Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
