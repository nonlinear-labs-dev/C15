#pragma once

#include <playground.h>
#include "TemplateEnums.h"
#include <tools/EnumTools.h>
#include <proxies/hwui/buttons.h>

namespace DescriptiveLayouts
{
  ENUM(EventSinks, uint8_t, IncParam, DecParam, SwitchToInitDetail, SwitchToEditMode, SwitchToSelectMode,
       SwitchToSetupFocus, SwitchToParameterFocus, SwitchToBankFocus, SwitchToPresetFocus, SwitchToSoundFocus,
       SwitchToMCSelectDetail, SwitchToButtonADetail, SwitchToButtonBDetail, SwitchToButtonCDetail,
       SwitchToButtonDDetail, SwitchToMCModRangeDetail, SwitchToMCAmtDetail, IncMCSel, DecMCSel, IncMCAmt, DecMCAmt,
       IncMCPos, DecMCPos, IncButtonMenu, FireButtonMenu);

  struct EventSinkMapping
  {
   public:
    EventSinkMapping(Buttons button, EventSinks sink, ButtonEvents event = ButtonEvents::Down,
                     ButtonModifiers modifiers = ButtonModifiers::None);

    Buttons button;
    ButtonEvents event;
    ButtonModifiers modifiers;
    EventSinks sink;
  };

  class EventSinkBroker
  {
   public:
    static EventSinkBroker& get();

    void fire(EventSinks s);

   private:
    using tAction = std::function<void()>;

    EventSinkBroker();
    void registerEvent(EventSinks sink, tAction action);

    std::unordered_map<EventSinks, tAction> m_map;
  };
}
