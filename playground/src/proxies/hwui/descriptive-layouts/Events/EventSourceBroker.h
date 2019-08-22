#pragma once

#include <playground.h>
#include <experimental/any>
#include <parameters/ValueRange.h>
#include <map>
#include "proxies/hwui/descriptive-layouts/TemplateEnums.h"

namespace DescriptiveLayouts
{
  class EventSourceBase;

  class EventSourceBroker
  {
   public:
    static EventSourceBroker& get();
    using Callback = std::function<void(std::experimental::any)>;

    sigc::connection connect(EventSources source, Callback cb);
    std::experimental::any evaluate(EventSources source);

   private:
    explicit EventSourceBroker();
    std::map<EventSources, std::unique_ptr<EventSourceBase>> m_map;
  };
}
