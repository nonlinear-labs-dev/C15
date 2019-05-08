#pragma once

#include "Dimension.h"

class NoteDimension : public Dimension
{
 private:
  NoteDimension();

 public:
  virtual ~NoteDimension();

  static const NoteDimension &get();
  Glib::ustring getStingizerJS() const override;
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
