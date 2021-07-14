#pragma once

#include "LinearScaleConverter.h"

class LinearBipolar50DbScaleConverter : public LinearScaleConverter
{
 public:
  LinearBipolar50DbScaleConverter();
  ~LinearBipolar50DbScaleConverter() override;
};
