#pragma once

#include "LinearScaleConverter.h"

class LinearBipolar25DbScaleConverter : public LinearScaleConverter
{
 public:
  LinearBipolar25DbScaleConverter();
  ~LinearBipolar25DbScaleConverter() override;
};
