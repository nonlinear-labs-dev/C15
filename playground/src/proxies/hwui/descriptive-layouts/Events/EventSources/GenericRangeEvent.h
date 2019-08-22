#pragma once
#include "proxies/hwui/descriptive-layouts/Events/EventSources/base/EventSource.h"

namespace DescriptiveLayouts
{
  class GenericRangeEvent : public EventSource<std::pair<tControlPositionValue, tControlPositionValue>>
  {

  };
}
