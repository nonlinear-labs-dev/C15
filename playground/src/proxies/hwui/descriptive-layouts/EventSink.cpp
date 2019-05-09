#include <parameters/ModulateableParameter.h>
#include <libundo/undo/Scope.h>
#include "EventSink.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/HWUI.h"

namespace DescriptiveLayouts
{
  EventSinkMapping::EventSinkMapping(Buttons button, EventSinks sink, ButtonEvents event, ButtonModifiers modifiers)
      : button(button)
      , sink(sink)
      , event(event)
      , modifiers(modifiers)
  {
  }

  EventSinkBroker& EventSinkBroker::get()
  {
    static EventSinkBroker s;
    return s;
  }

  EventSinkBroker::EventSinkBroker()
  {
    auto eb = Application::get().getPresetManager()->getEditBuffer();
    auto hwui = Application::get().getHWUI();

    registerEvent(EventSinks::IncParam, [eb, hwui]() {
      if(auto p = eb->getSelected())
        p->getValue().inc(Initiator::EXPLICIT_HWUI, hwui->getButtonModifiers());
    });

    registerEvent(EventSinks::IncMCPos, [eb, hwui]() {
      if(auto mc = dynamic_cast<ModulateableParameter*>(eb->getSelected())->getMacroControl())
      {
        UNDO::Scope::tTransactionScopePtr rootScope = eb->getParent()->getUndoScope().startTransaction("Big Bang");
        auto trans = rootScope->getTransaction();
        mc->stepCPFromHwui(trans, 1, hwui->getButtonModifiers());
      }
    });

    registerEvent(EventSinks::DecParam, [eb, hwui]() {
      if(auto p = eb->getSelected())
        p->getValue().dec(Initiator::EXPLICIT_HWUI, hwui->getButtonModifiers());
    });

    registerEvent(EventSinks::SwitchToEditMode, [hwui]() { hwui->undoableSetFocusAndMode(UIMode::Edit); });

    registerEvent(EventSinks::SwitchToSelectMode, [hwui]() { hwui->undoableSetFocusAndMode(UIMode::Select); });

    registerEvent(EventSinks::SwitchToMCSelectDetail, [hwui, eb]() {
      if(dynamic_cast<ModulateableParameter*>(eb->getSelected()) != nullptr)
      {
        hwui->setUiModeDetail(UIDetail::MCSelect);
      }
    });

    registerEvent(EventSinks::SwitchToInitDetail, [hwui]() { hwui->setUiModeDetail(UIDetail::Init); });

    registerEvent(EventSinks::SwitchToMCModRangeDetail, [hwui, eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter*>(eb->getSelected()))
      {
        if(modParam->getModulationSource() != ModulationSource::NONE)
        {
          hwui->setUiModeDetail(UIDetail::MCModRange);
        }
      }
    });

    registerEvent(EventSinks::IncMCSel, [eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter*>(eb->getSelected()))
        modParam->undoableIncrementMCSelect(1);
    });

    registerEvent(EventSinks::DecMCSel, [eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter*>(eb->getSelected()))
        modParam->undoableIncrementMCSelect(-1);
    });

    registerEvent(EventSinks::SwitchToMCAmtDetail, [hwui, eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter*>(eb->getSelected()))
      {
        if(modParam->getModulationSource() != ModulationSource::NONE)
        {
          hwui->setUiModeDetail(UIDetail::MCAmount);
        }
      }
    });

    registerEvent(EventSinks::DecMCAmt, [eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter*>(eb->getSelected()))
      {
        modParam->undoableIncrementMCAmount(-1, {});
      }
    });

    registerEvent(EventSinks::IncMCAmt, [eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter*>(eb->getSelected()))
      {
        modParam->undoableIncrementMCAmount(1, {});
      }
    });
  }

  void EventSinkBroker::registerEvent(EventSinks sink, tAction action)
  {
    m_map[sink] = action;
  }

  void EventSinkBroker::fire(EventSinks s)
  {
    m_map.at(s)();
  }
}
