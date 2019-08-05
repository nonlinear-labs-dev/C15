#pragma once

#include <proxies/hwui/DFBLayout.h>
#include <nltools/threading/Throttler.h>
#include "LayoutClass.h"

namespace DescriptiveLayouts
{
  class PropertyOwner;

  class GenericLayout : public DFBLayout
  {
    using super = DFBLayout;

   public:
    GenericLayout(LayoutClass prototype);
    ~GenericLayout();

    void init() override;
    bool onButton(Buttons i, bool down, ::ButtonModifiers modifiers) override;
    bool onRotary(int inc, ::ButtonModifiers modifiers) override;

    bool regularRedraw();
   private:
    bool handleEventSink(EventSinks s);
    void createControls();

    LayoutClass m_prototype;
    bool handleDefaults(Buttons buttons, bool down, ::ButtonModifiers modifiers);
    void togglePresetMode();
    void toggleSoundMode();
    void toggleSetupMode();
    void toggleStoreMode();
    void toggleInfo();
    void toggleEdit();

    mutable Throttler m_throttler;
    sigc::connection m_redrawHandle;
  };
}
