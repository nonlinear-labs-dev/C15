#pragma once

#include "LinearScaleConverter.h"

class LinearBipolar48DbScaleConverter : public LinearScaleConverter
{
 public:
  LinearBipolar48DbScaleConverter();
  ~LinearBipolar48DbScaleConverter() override;
};
