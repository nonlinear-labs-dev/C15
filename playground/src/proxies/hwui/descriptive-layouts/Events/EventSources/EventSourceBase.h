#pragma once
#include <playground.h>
#include <experimental/any>
#include <parameters/ValueRange.h>
#include <map>
#include "proxies/hwui/descriptive-layouts/TemplateEnums.h"

namespace DescriptiveLayouts
{

  class EventSourceBase : public sigc::trackable
  {
   public:
    using Callback = std::function<void(std::experimental::any)>;
    sigc::connection connect(const Callback &cb);
   protected:
    virtual std::experimental::any getLastValue() const = 0;
    Signal<void, std::experimental::any> m_outputSignal;
    friend class EventSourceBroker;
  };
}