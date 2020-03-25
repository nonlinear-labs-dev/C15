#pragma once

#include <proxies/hwui/Layout.h>
#include <nltools/threading/Throttler.h>
#include "LayoutClass.h"

namespace DescriptiveLayouts
{
  class PropertyOwner;
  class EventProvider;

  class GenericLayout : public Layout
  {
    using super = Layout;

   public:
    explicit GenericLayout(LayoutClass prototype);
    ~GenericLayout() override;

    void init() override;
    bool onButton(Buttons i, bool down, ::ButtonModifiers modifiers) override;
    bool onRotary(int inc, ::ButtonModifiers modifiers) override;
    const LayoutClass& getPrototype() const;

   private:
    bool handleEventSink(EventSinks s);
    void createControls();
    void connectConditions();
    bool handleDefaults(Buttons buttons, bool down, ::ButtonModifiers modifiers);
    void togglePresetMode();
    void toggleSoundMode();
    void toggleSetupMode();
    void toggleStoreMode();
    void toggleInfo();
    void toggleEdit();

    LayoutClass m_prototype;
    std::unique_ptr<EventProvider> m_eventProvider;
    sigc::connection m_onConditionsChanged;
  };
}
