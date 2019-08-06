#pragma once

#include "ControlInstance.h"
#include "proxies/hwui/controls/ControlWithChildren.h"

namespace DescriptiveLayouts
{
  class GenericLayout;

  class GenericControl : public ControlWithChildren
  {
   public:
    GenericControl(const ControlInstance& prototype);
    virtual ~GenericControl();

    void style(const LayoutClasses& layout);
    void connect();

   private:
    void addPrimitives();
    void onEventFired(const std::any& v, const ControlInstance::EventConnection& c);
    void onVisibilityChanged(std::any visibility, const ControlInstance& ci, const ControlInstance::VisibilityItem& item);

  private:

    std::list<sigc::connection> m_connections;

    std::unordered_map<EventSources, bool> m_controlVisible;

  protected:
    ControlInstance m_prototype;
    LayoutClasses m_lastUsedLayout;
  };
}
