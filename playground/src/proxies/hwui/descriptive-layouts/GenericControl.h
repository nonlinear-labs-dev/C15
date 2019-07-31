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

    void style(LayoutClasses layout);
    void connect();

    bool drawBackgroundTransparent(FrameBuffer &fb) const;
    bool redraw(FrameBuffer& fb) override;

   private:
    void addPrimitives();
    void onEventFired(std::any v, const ControlInstance::EventConnection& c);
    void onVisibilityChanged(std::any visibility, const ControlInstance& ci, const ControlInstance::VisibilityItem& item);

    ControlInstance m_prototype;
    std::list<sigc::connection> m_connections;
    bool m_controlVisible = true;
  protected:
    LayoutClasses m_lastUsedLayout;
  };
}
