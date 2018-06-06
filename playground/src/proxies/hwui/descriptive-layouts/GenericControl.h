#pragma once

#include "ControlInstance.h"
#include "proxies/hwui/controls/ControlWithChildren.h"

namespace DescriptiveLayouts
{
  class GenericControl : public ControlWithChildren
  {
    public:
      GenericControl(const ControlInstance &prototype);
      virtual ~GenericControl();

      void style(LayoutClasses layout);
      void connect();

    private:
      void addPrimitives();
      void onEventFired(std::any v, const ControlInstance::EventConnection &c);

      ControlInstance m_prototype;
      std::list<sigc::connection> m_connections;
  };

}
