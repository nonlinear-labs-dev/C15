#pragma once

#include "LinearScaleConverter.h"

class LinearMinus50To0DbScaleConverter : public LinearScaleConverter
{
 public:
  LinearMinus50To0DbScaleConverter();
  virtual ~LinearMinus50To0DbScaleConverter();
};
