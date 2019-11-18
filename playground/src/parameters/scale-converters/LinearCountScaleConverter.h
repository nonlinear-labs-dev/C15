#pragma once

template <int tCount, typename tDimension> class LinearCountScaleConverter : public LinearScaleConverter
{
 public:
  LinearCountScaleConverter()
      : LinearScaleConverter(tTcdRange(1, tCount), tDisplayRange(1, tCount), tDimension::get())
  {
  }
};