#pragma once

#include "LinearScaleConverter.h"

class FineBipolar12STScaleConverter : public LinearScaleConverter
{
 private:
  typedef LinearScaleConverter super;

 public:
  FineBipolar12STScaleConverter();
  virtual ~FineBipolar12STScaleConverter();

 private:
  FineBipolar12STScaleConverter(const FineBipolar12STScaleConverter& other);
  FineBipolar12STScaleConverter& operator=(const FineBipolar12STScaleConverter&);
};
