#pragma once

#include "LinearScaleConverter.h"

class LinearBipolar60DbtScaleConverter : public LinearScaleConverter
{
 public:
  LinearBipolar60DbtScaleConverter();
  ~LinearBipolar60DbtScaleConverter() override;
};
