#pragma once

#include "LinearScaleConverter.h"

class FineBipolar12STScaleConverter : public LinearScaleConverter
{
 private:
  typedef LinearScaleConverter super;

 public:
  FineBipolar12STScaleConverter();
  ~FineBipolar12STScaleConverter() override;

 private:
  FineBipolar12STScaleConverter(const FineBipolar12STScaleConverter& other);
  FineBipolar12STScaleConverter& operator=(const FineBipolar12STScaleConverter&);
};
