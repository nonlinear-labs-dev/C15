#pragma once

#include "LinearScaleConverter.h"

class LinearBipolar60DbScaleConverter : public LinearScaleConverter
{
 public:
  LinearBipolar60DbScaleConverter();
  ~LinearBipolar60DbScaleConverter() override;
};