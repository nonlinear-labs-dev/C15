#include <utility>

#include <parameters/ModulateableParameter.h>
#include <libundo/undo/Scope.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetList.h>
#include "EventSink.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/HWUI.h"
#include "proxies/hwui/Layout.h"
#include "GenericLayout.h"

namespace DescriptiveLayouts
{
  EventSinkMapping::EventSinkMapping(Buttons button, EventSinks sink, ButtonEvents event, ButtonModifiers modifiers)
      : button(button)
      , sink(sink)
      , event(event)
      , modifiers(modifiers)
  {
  }

  EventSinkBroker &EventSinkBroker::get()
  {
    static EventSinkBroker s;
    return s;
  }

  EventSinkBroker::EventSinkBroker()
  {
    auto eb = Application::get().getPresetManager()->getEditBuffer();
    auto hwui = Application::get().getHWUI();

    /*
       * Basic Parameter Events
       */
    registerEvent(EventSinks::IncParam, [eb, hwui]() {
      if(auto p = eb->getSelected())
        p->getValue().inc(Initiator::EXPLICIT_HWUI, hwui->getButtonModifiers());
    });

    registerEvent(EventSinks::DecParam, [eb, hwui]() {
      if(auto p = eb->getSelected())
        p->getValue().dec(Initiator::EXPLICIT_HWUI, hwui->getButtonModifiers());
    });

    /*
       * Modulation Events
       */
    registerEvent(EventSinks::IncMCSel, [eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected()))
        modParam->undoableIncrementMCSelect(1);
    });

    registerEvent(EventSinks::DecMCSel, [eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected()))
        modParam->undoableIncrementMCSelect(-1);
    });

    registerEvent(EventSinks::IncMCAmt, [eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected()))
      {
        modParam->undoableIncrementMCAmount(1, {});
      }
    });

    registerEvent(EventSinks::DecMCAmt, [eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected()))
      {
        modParam->undoableIncrementMCAmount(-1, {});
      }
    });

    registerEvent(EventSinks::IncMCPos, [eb, hwui]() {
      if(auto modP = dynamic_cast<ModulateableParameter *>(eb->getSelected()))
      {
        if(auto mc = modP->getMacroControl())
        {
          UNDO::Scope::tTransactionScopePtr rootScope = eb->getParent()->getUndoScope().startTransaction("Inc. MC Pos");
          auto trans = rootScope->getTransaction();
          mc->stepCPFromHwui(trans, 1, hwui->getButtonModifiers());
        }
      }
    });

    registerEvent(EventSinks::DecMCPos, [eb, hwui]() {
      if(auto modP = dynamic_cast<ModulateableParameter *>(eb->getSelected()))
      {
        if(auto mc = modP->getMacroControl())
        {
          UNDO::Scope::tTransactionScopePtr rootScope = eb->getParent()->getUndoScope().startTransaction("Dec. MC Pos");
          auto trans = rootScope->getTransaction();
          mc->stepCPFromHwui(trans, -1, hwui->getButtonModifiers());
        }
      }
    });

    registerEvent(EventSinks::ToggleVoiceGroup, [eb]() {
      if(eb->getType() != Type::Single)
      {
        eb->m_vgISelected = !eb->m_vgISelected;
        eb->onChange();
      }
    });

    /*
       * UIFocus
       */
    registerEvent(EventSinks::SwitchToParameterFocus, [hwui] { hwui->undoableSetFocusAndMode(UIFocus::Parameters); });
    registerEvent(EventSinks::SwitchToSoundFocus, [hwui] {
      hwui->undoableSetFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
    });
    registerEvent(EventSinks::SwitchToPresetFocus, [hwui] { hwui->undoableSetFocusAndMode(UIFocus::Presets); });
    registerEvent(EventSinks::SwitchToBankFocus, [hwui] { hwui->undoableSetFocusAndMode(UIFocus::Banks); });
    registerEvent(EventSinks::SwitchToSetupFocus, [hwui] { hwui->undoableSetFocusAndMode(UIFocus::Setup); });

    /*
       * UIMode
       */
    registerEvent(EventSinks::SwitchToEditMode, [hwui]() { hwui->undoableSetFocusAndMode(UIMode::Edit); });
    registerEvent(EventSinks::SwitchToSelectMode, [hwui]() { hwui->undoableSetFocusAndMode(UIMode::Select); });

    /*
       * UIDetail
       */
    registerEvent(EventSinks::SwitchToInitDetail, [hwui]() { hwui->setUiModeDetail(UIDetail::Init); });
    registerEvent(EventSinks::SwitchToButtonADetail, [hwui] { hwui->setUiModeDetail(UIDetail::ButtonA); });
    registerEvent(EventSinks::SwitchToButtonBDetail, [hwui] { hwui->setUiModeDetail(UIDetail::ButtonB); });
    registerEvent(EventSinks::SwitchToButtonCDetail, [hwui] { hwui->setUiModeDetail(UIDetail::ButtonC); });
    registerEvent(EventSinks::SwitchToButtonDDetail, [hwui] { hwui->setUiModeDetail(UIDetail::ButtonD); });

    registerEvent(EventSinks::SelectPresetForVoiceGroup, [hwui] {
      hwui->setUiModeDetail(UIDetail::SoundSelectPresetForVoiceGroup);
      hwui->getPanelUnit().getEditPanel().getBoled().bruteForce();
    });

    registerEvent(EventSinks::ToggleDirectLoad,
                  [] { Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->toggle(); });

    registerEvent(EventSinks::SwitchToMCAmtDetail, [hwui, eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected()))
      {
        if(modParam->getModulationSource() != MacroControls::NONE)
        {
          hwui->setUiModeDetail(UIDetail::MCAmount);
        }
      }
    });

    registerEvent(EventSinks::SwitchToMCModRangeDetail, [hwui, eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected()))
      {
        if(modParam->getModulationSource() != MacroControls::NONE)
        {
          hwui->setUiModeDetail(UIDetail::MCModRange);
        }
      }
    });

    registerEvent(EventSinks::SwitchToMCSelectDetail, [hwui, eb]() {
      if(dynamic_cast<ModulateableParameter *>(eb->getSelected()) != nullptr)
      {
        hwui->setUiModeDetail(UIDetail::MCSelect);
      }
    });

    registerEvent(EventSinks::IncParamSelection, [hwui, eb]() {
      auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout().get();
      if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
      {
        if(auto carousel = genericLayout->findControlOfType<ParameterCarousel>())
        {
          carousel->turn();
        }
      }
    });

    registerEvent(EventSinks::IncModulationCarousel, [hwui]() {
      auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout().get();
      if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
      {
        if(auto carousel = genericLayout->findControlOfType<ModulationCarousel>())
        {
          carousel->onRotary(1, hwui->getButtonModifiers());
        }
      }
    });

    registerEvent(EventSinks::DecModulationCarousel, [hwui]() {
      auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout().get();
      if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
      {
        if(auto carousel = genericLayout->findControlOfType<ModulationCarousel>())
        {
          carousel->onRotary(-1, hwui->getButtonModifiers());
        }
      }
    });

      registerEvent(EventSinks::IncPresetSelectionPresetList, [hwui]() {
          auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout().get();
          if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
          {
            if(auto presetList = genericLayout->findControlOfType<GenericPresetList>())
            {
              presetList->incPresetSelection();
            }
          }
      });

      registerEvent(EventSinks::DecPresetSelectionPresetList, [hwui]() {
          auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout().get();
          if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
          {
              if(auto presetList = genericLayout->findControlOfType<GenericPresetList>())
              {
                presetList->decPresetSelection();
              }
          }
      });

    registerEvent(EventSinks::DecBankSelectionPresetList, [hwui]() {
        auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout().get();
        if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
        {
          if(auto presetList = genericLayout->findControlOfType<GenericPresetList>())
          {
            presetList->decBankSelection();
          }
        }
    });

    registerEvent(EventSinks::IncBankSelectionPresetList, [hwui]() {
        auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout().get();
        if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
        {
          if(auto presetList = genericLayout->findControlOfType<GenericPresetList>())
          {
            presetList->incBankSelection();
          }
        }
    });

    registerEvent(EventSinks::DoPresetListAction, [hwui]() {
      auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout().get();
      if(auto genericLayout = dynamic_cast<GenericLayout* >(layout))
      {
        if(auto presetList = genericLayout->findControlOfType<GenericPresetList>())
        {
          presetList->action();
        }
      }
    });
  }

  void EventSinkBroker::registerEvent(EventSinks sink, tAction action)
  {
    m_map[sink] = std::move(action);
  }

  void EventSinkBroker::fire(EventSinks s)
  {
    m_map.at(s)();
  }
}
