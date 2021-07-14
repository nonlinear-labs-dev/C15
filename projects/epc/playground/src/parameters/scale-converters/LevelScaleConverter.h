#pragma once

#include "LinearScaleConverter.h"

class LevelScaleConverter : public LinearScaleConverter
{
  typedef LinearScaleConverter super;

 public:
  LevelScaleConverter();
  ~LevelScaleConverter() override;
};
