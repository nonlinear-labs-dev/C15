#pragma once

#include "Dimension.h"

class DBPerSTDimension : public Dimension
{
 public:
  virtual ~DBPerSTDimension();

  static const DBPerSTDimension &get();
  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  DBPerSTDimension();
};
