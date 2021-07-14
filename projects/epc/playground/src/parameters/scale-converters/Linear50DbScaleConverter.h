#pragma once

#include "LinearScaleConverter.h"

class Linear50DbScaleConverter : public LinearScaleConverter
{
 public:
  Linear50DbScaleConverter();
  ~Linear50DbScaleConverter() override;
};
