#pragma once

#include <playground.h>
#include <any>
#include <parameters/ValueRange.h>
#include <unordered_map>
#include "TemplateEnums.h"

namespace DescriptiveLayouts
{
  class EventSourceBase : public sigc::trackable
  {
    public:
      using Callback = std::function<void (std::any)>;

      sigc::connection connect(Callback cb)
      {
        cb(getLastValue());
        return m_outputSignal.connect(cb);
      }

    protected:
      virtual std::any getLastValue() const = 0;
      Signal<void, std::any> m_outputSignal;
  };

  class EventSourceBroker
  {
    public:
      static EventSourceBroker& get();
      using Callback = std::function<void (std::any)>;

      sigc::connection connect(EventSources source, Callback cb);

    private:
      EventSourceBroker();
      std::unordered_map<EventSources, std::unique_ptr<EventSourceBase>> m_map;
  };
}
