#pragma once

#include "Dimension.h"

class DBPerSTDimension : public Dimension
{
 public:
  virtual ~DBPerSTDimension();

  static const DBPerSTDimension &get();
  Glib::ustring getStingizerJS() const override;

  virtual Glib::ustring stringize(const tDisplayValue &displayValue) const override;

 private:
  DBPerSTDimension();
};
