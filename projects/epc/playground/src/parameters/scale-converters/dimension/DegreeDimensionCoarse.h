#pragma once

#include "Dimension.h"

class DegreeDimensionCoarse : public Dimension
{
 private:
  DegreeDimensionCoarse();

 public:
  virtual ~DegreeDimensionCoarse();

  static const DegreeDimensionCoarse &get();
  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
