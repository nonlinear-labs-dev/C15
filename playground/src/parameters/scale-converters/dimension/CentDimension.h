#pragma once

#include "Dimension.h"

class CentDimension : public Dimension
{
 private:
  CentDimension();

 public:
  virtual ~CentDimension();

  static const CentDimension &get();
  Glib::ustring getStingizerJS() const override;

  Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
