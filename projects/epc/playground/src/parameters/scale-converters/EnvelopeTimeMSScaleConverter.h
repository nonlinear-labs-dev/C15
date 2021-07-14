#pragma once

#include "TimeScaleConverter.h"

class EnvelopeTimeMSScaleConverter : public TimeScaleConverter
{
  typedef TimeScaleConverter super;

 public:
  EnvelopeTimeMSScaleConverter();
  ~EnvelopeTimeMSScaleConverter() override;
};
