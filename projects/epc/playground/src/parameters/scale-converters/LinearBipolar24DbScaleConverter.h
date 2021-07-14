#pragma once

#include "LinearScaleConverter.h"

class LinearBipolar24DbScaleConverter : public LinearScaleConverter
{
 public:
  LinearBipolar24DbScaleConverter();
  ~LinearBipolar24DbScaleConverter() override;
};
