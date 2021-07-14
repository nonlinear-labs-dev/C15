#pragma once

#include "LinearScaleConverter.h"

class Linear60DbtScaleConverter : public LinearScaleConverter
{
 public:
  Linear60DbtScaleConverter();
  ~Linear60DbtScaleConverter() override;
};
