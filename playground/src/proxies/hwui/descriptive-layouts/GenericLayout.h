#pragma once

#include <proxies/hwui/DFBLayout.h>
#include "LayoutClass.h"


namespace DescriptiveLayouts
{
  class PropertyOwner;

  class GenericLayout : public DFBLayout
  {
      using super = DFBLayout;

    public:
      GenericLayout(const LayoutClass &prototype);

      void init () override;
      bool onButton (Buttons i, bool down, ::ButtonModifiers modifiers) override;
      bool onRotary (int inc, ::ButtonModifiers modifiers) override;

    private:
      bool handleEventSink(EventSinks s);
      void createControls();

      LayoutClass m_prototype;
  };
}
