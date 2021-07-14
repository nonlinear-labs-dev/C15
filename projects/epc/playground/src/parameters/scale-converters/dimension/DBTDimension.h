#pragma once

#include "Dimension.h"

class DBTDimension : public Dimension
{
 public:
  virtual ~DBTDimension();

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  static const DBTDimension &get();
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  DBTDimension();
};
