#pragma once

#include "Dimension.h"

class NoteDimension : public Dimension
{
 private:
  NoteDimension();

 public:
  virtual ~NoteDimension();

  static const NoteDimension &get();

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
