#pragma once

#include "Dimension.h"

class LevelDimension : public Dimension
{
 public:
  LevelDimension();
  virtual ~LevelDimension();

  static tDisplayValue silence()
  {
    return -128.0;
  }

  static const LevelDimension &get();
  Glib::ustring getStingizerJS() const override;

  Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
