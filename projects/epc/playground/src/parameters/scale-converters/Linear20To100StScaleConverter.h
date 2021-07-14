#pragma once

#include "LinearScaleConverter.h"

class Linear20To100StScaleConverter : public LinearScaleConverter
{
 public:
  Linear20To100StScaleConverter();
  ~Linear20To100StScaleConverter() override;
};
