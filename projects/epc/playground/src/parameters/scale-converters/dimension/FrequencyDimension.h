#pragma once

#include "Dimension.h"

class FrequencyDimension : public Dimension
{
 public:
  FrequencyDimension();
  virtual ~FrequencyDimension();

  static const FrequencyDimension &get();
  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
