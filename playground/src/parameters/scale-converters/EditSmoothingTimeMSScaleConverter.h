#pragma once

#include "LinearScaleConverter.h"

class EditSmoothingTimeMSScaleConverter : public LinearScaleConverter
{
  typedef LinearScaleConverter super;

 public:
  EditSmoothingTimeMSScaleConverter();
  virtual ~EditSmoothingTimeMSScaleConverter();
};
