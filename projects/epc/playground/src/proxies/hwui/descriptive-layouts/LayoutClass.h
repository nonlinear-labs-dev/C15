#pragma once

#include <utility>
#include <proxies/hwui/HWUIEnums.h>
#include "Selector.h"
#include "ControlInstance.h"
#include <set>
#include <proxies/hwui/descriptive-layouts/conditions/ConditionBase.h>
#include <proxies/hwui/descriptive-layouts/TemplateEnums.h>

class Layout;

namespace DescriptiveLayouts
{

  class ControlInstance;

  struct LayoutClass
  {
   public:
    using ControlInstanceList = std::list<ControlInstance>;
    using EventSinkList = std::list<EventSinkMapping>;
    using ConditionList = std::list<ConditionBase *>;

    template <typename... Args>
    explicit LayoutClass(LayoutClasses id, Args... args)
        : id(std::move(id))
    {
      (void) std::initializer_list<int> { (addToList(args), 0)... };
    }

    LayoutClass(LayoutClasses id, std::list<Selector> sel, std::list<ControlInstance> ci,
                std::list<EventSinkMapping> esm, const ConditionList &con, EventProviders ep)
        : id(std::move(id))
        , eventProvider(ep)
    {
      for(const auto &s : sel)
        addToList(s);
      for(const auto &c : ci)
        addToList(c);
      for(auto e : esm)
        addToList(e);
      for(auto c : con)
        addToList(c);
    }

    bool matches(FocusAndMode fam) const;
    bool meetsConditions() const;
    long getTotalWeight() const;
    std::pair<long, long> getWeight() const;
    const std::string getName() const;

    FocusAndMode getDesiredFocusAndMode() const;

    Layout *instantiate() const;

   private:
    void addToList(Selector s);
    void addToList(ControlInstance s);
    void addToList(EventSinkMapping s);
    void addToList(ConditionBase *c);

    LayoutClasses id;
    std::list<Selector> selectors;
    ConditionList conditions;
    ControlInstanceList controls;
    EventSinkList sinkMappings;
    EventProviders eventProvider;

    friend class GenericLayout;
    friend class ConsistencyChecker;
  };
}
