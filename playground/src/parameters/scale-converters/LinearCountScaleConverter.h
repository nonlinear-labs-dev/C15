#pragma once
#include "LinearScaleConverter.h"

template <int tCount, typename tDimension> class LinearCountScaleConverter : public LinearScaleConverter
{
 public:
  LinearCountScaleConverter()
      : LinearScaleConverter(tTcdRange(0, tCount - 1), tDisplayRange(0, tCount - 1), tDimension::get())
  {
  }
};