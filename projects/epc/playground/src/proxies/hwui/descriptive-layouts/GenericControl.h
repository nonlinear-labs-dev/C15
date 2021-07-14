#pragma once

#include "ControlInstance.h"
#include "proxies/hwui/controls/ControlWithChildren.h"
#include <sigc++/connection.h>

namespace DescriptiveLayouts
{
  class GenericLayout;
  class EventProvider;

  class GenericControl : public ControlWithChildren
  {
   public:
    GenericControl(const ControlInstance& prototype, EventProvider* eventProvider);
    ~GenericControl() override;

    void style(const LayoutClasses& layout);
    void connect();

   private:
    void addPrimitives();
    void onEventFired(const std::experimental::any& v, const ControlInstance::EventConnection& c);

   private:
    std::list<sigc::connection> m_connections;

   protected:
    bool isTransparent() const override;

    ControlInstance m_prototype;
    LayoutClasses m_lastUsedLayout;
    EventProvider* m_eventProvider = nullptr;
  };
}
