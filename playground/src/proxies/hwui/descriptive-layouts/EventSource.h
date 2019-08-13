#pragma once

#include <playground.h>
#include <experimental/any>
#include <parameters/ValueRange.h>
#include <unordered_map>
#include "TemplateEnums.h"

namespace DescriptiveLayouts
{
  class EventSourceBroker;

  class EventSourceBase : public sigc::trackable
  {
   public:
    using Callback = std::function<void(std::experimental::any)>;

    sigc::connection connect(const Callback& cb)
    {
      cb(getLastValue());
      return m_outputSignal.connect(cb);
    }

   protected:
    virtual std::experimental::any getLastValue() const = 0;
    Signal<void, std::experimental::any> m_outputSignal;

    friend class EventSourceBroker;
  };

  class EventSourceBroker
  {
   public:
    static EventSourceBroker& get();
    using Callback = std::function<void(std::experimental::any)>;

    sigc::connection connect(EventSources source, Callback cb);
    std::experimental::any evaluate(EventSources source);

   private:
    explicit EventSourceBroker();
    std::unordered_map<EventSources, std::unique_ptr<EventSourceBase>> m_map;
  };
}
