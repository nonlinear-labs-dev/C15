#pragma once

#include "Dimension.h"

class DBTDimension : public Dimension
{
 public:
  virtual ~DBTDimension();

  Glib::ustring getStingizerJS() const override;
  static const DBTDimension &get();
  Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  DBTDimension();
};
