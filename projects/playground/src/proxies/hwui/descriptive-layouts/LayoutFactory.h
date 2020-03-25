#pragma once

#include <cstring>
#include <assert.h>
#include <list>
#include <proxies/hwui/Layout.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/controls/Rect.h>
#include "GenericLayout.h"
#include "proxies/hwui/descriptive-layouts/events/GlobalEventSourceBroker.h"
#include "Selector.h"
#include "LayoutClass.h"
#include "device-settings/DebugLevel.h"

namespace DescriptiveLayouts
{
  class GenericControl;

  class BoledLayoutFactory
  {
   public:
    static BoledLayoutFactory& get();

    void clear();

    std::shared_ptr<Layout> instantiate(FocusAndMode fam);

    template <typename... Args>

    void registerLayout(LayoutClasses id, Args... args)
    {
      m_layouts.emplace_back(id, args...);
      DebugLevel::gassy("registered Layout:", id);
    }

    void registerLayout(LayoutClasses id, std::list<Selector> sel, std::list<ControlInstance> ci,
                        std::list<EventSinkMapping> esm, std::list<ConditionBase*> con, EventProviders ep)
    {
      m_layouts.emplace_back(id, sel, ci, esm, con, ep);
      DebugLevel::gassy("registered Layout:", id);
    }
    void sortByPriority();

   private:
    BoledLayoutFactory();

    const LayoutClass& find(FocusAndMode fam) const;

    std::list<LayoutClass> m_layouts;

    friend class ConsistencyChecker;
  };
}
