#pragma once

#include "LinearScaleConverter.h"

class LinearMinus50To0DbScaleConverter : public LinearScaleConverter
{
 public:
  LinearMinus50To0DbScaleConverter();
  ~LinearMinus50To0DbScaleConverter() override;
};
