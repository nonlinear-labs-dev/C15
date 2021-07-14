#pragma once

#include "LinearScaleConverter.h"

class Linear24To120StScaleConverter : public LinearScaleConverter
{
 public:
  Linear24To120StScaleConverter();
  ~Linear24To120StScaleConverter() override;
};
