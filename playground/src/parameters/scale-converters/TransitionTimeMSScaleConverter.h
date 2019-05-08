#pragma once

#include "TimeScaleConverter.h"

class TransitionTimeMSScaleConverter : public TimeScaleConverter
{
  typedef TimeScaleConverter super;

 public:
  TransitionTimeMSScaleConverter();
  virtual ~TransitionTimeMSScaleConverter();
};
