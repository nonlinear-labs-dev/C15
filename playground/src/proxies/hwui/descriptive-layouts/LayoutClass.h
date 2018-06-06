#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include "Selector.h"
#include "EventSink.h"
#include "ControlInstance.h"

class DFBLayout;

namespace DescriptiveLayouts
{

  class ControlInstance;

  struct LayoutClass
  {
    public:
      using ControlInstanceList = std::list<ControlInstance>;
      using EventSinkList = std::list<EventSinkMapping>;
      using ConditionList = std::list<std::function<bool()>>;

      template<typename ... Args>
        LayoutClass(LayoutClasses id, Args ... args) :
            id(id)
        {
          (void) std::initializer_list<int> { (addToList(args), 0)... };
        }

      LayoutClass(LayoutClasses id, std::list<Selector> sel,
                      std::list<ControlInstance> ci,
                      std::list<EventSinkMapping> esm,
                      ConditionList con) : id(id)
      {
        for(auto s: sel)
          addToList(s);
        for(auto c: ci)
          addToList(c);
        for(auto e: esm)
          addToList(e);
        for(auto c: con)
          addToList(c);
      }

      bool matches(FocusAndMode fam) const;
      bool meetsConditions() const;
      const unsigned long getWeight() const;

      DFBLayout* instantiate() const;

    private:
      void addToList(Selector s);
      void addToList(ControlInstance s);
      void addToList(EventSinkMapping s);
      void addToList(std::function<bool()> c);

      LayoutClasses id;
      std::list<Selector> selectors;
      ConditionList conditions;
      ControlInstanceList controls;
      EventSinkList sinkMappings;

      friend class GenericLayout;
  };

}
