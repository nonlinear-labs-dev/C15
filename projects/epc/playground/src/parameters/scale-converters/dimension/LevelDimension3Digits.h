#pragma once

#include "Dimension.h"

class LevelDimension3Digits : public Dimension
{
 public:
  LevelDimension3Digits();
  virtual ~LevelDimension3Digits();

  static tDisplayValue silence()
  {
    return -128.0;
  }

  static const LevelDimension3Digits &get();

  [[nodiscard]] Glib::ustring getStingizerJS() const override;
  [[nodiscard]] Glib::ustring stringize(const tDisplayValue &displayValue) const override;
};
