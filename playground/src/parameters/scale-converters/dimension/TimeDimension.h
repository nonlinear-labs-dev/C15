#pragma once

#include "Dimension.h"

class TimeDimension : public Dimension
{
 public:
  TimeDimension();
  virtual ~TimeDimension();

  Glib::ustring getStingizerJS() const override;
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;

  static const TimeDimension &get();
};
