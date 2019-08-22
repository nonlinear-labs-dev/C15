#pragma once
#include "ParameterEvent.h"

namespace DescriptiveLayouts
{
  class SliderRangeEventSource : public ParameterEvent<Range>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override
    {
      auto v = p->getControlPositionValue();
      if(p->isBiPolar())
      {
        auto value = (v + 1) / 2;
        setValue(std::make_pair(0.5, value));
      }
      else
      {
        setValue(std::make_pair(0, v));
      }
    }
  };
}