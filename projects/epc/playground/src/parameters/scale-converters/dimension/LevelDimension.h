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

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
