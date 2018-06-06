#pragma once

#include <cstring>
#include <assert.h>
#include <list>
#include <proxies/hwui/DFBLayout.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/controls/Rect.h>
#include "GenericLayout.h"
#include "EventSink.h"
#include "EventSource.h"
#include "Selector.h"
#include "LayoutClass.h"

namespace DescriptiveLayouts
{
  class GenericControl;

  class BoledLayoutFactory
  {
    public:
      static BoledLayoutFactory& get();

      void clear();

      std::shared_ptr<DFBLayout> instantiate(FocusAndMode fam);

      template<typename ... Args>
      void registerLayout(LayoutClasses id, Args ... args)
      {
        m_layouts.emplace_back(id, args...);
        DebugLevel::warning("registered Layout:", id);
      }

      void registerLayout(LayoutClasses id,
                          std::list<Selector> sel,
                          std::list<ControlInstance> ci,
                          std::list<EventSinkMapping> esm,
                          std::list<std::function<bool()>> con) {
        m_layouts.emplace_back(id, sel, ci, esm, con);
        DebugLevel::warning("registered Layout:", id);
      }
      void sortByPriority();
    private:
      BoledLayoutFactory();




      const LayoutClass& find(FocusAndMode fam) const;

      std::list<LayoutClass> m_layouts;
  };
}
