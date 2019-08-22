#pragma once
#include "EventSource.h"

namespace DescriptiveLayouts
{
  class GenericRangeEvent : public EventSource<std::pair<tControlPositionValue, tControlPositionValue>>
  {

  };
}
