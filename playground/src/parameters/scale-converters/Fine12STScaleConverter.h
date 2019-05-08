#pragma once

#include "LinearScaleConverter.h"

class Fine12STScaleConverter : public LinearScaleConverter
{
 private:
  typedef LinearScaleConverter super;

 public:
  Fine12STScaleConverter();
  virtual ~Fine12STScaleConverter();
};
