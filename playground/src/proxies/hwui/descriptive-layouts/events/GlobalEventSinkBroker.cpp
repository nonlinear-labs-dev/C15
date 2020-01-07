#include <utility>
#include "GlobalEventSinkBroker.h"
#include <parameters/ModulateableParameter.h>
#include <libundo/undo/Scope.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <device-settings/LoadModeSetting.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetList.h>
#include <proxies/hwui/descriptive-layouts/concrete/preset/GenericPresetList.h>
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/HWUI.h"
#include "proxies/hwui/Layout.h"
#include <proxies/hwui/descriptive-layouts/GenericLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/DualVoiceGroupMasterAndSplitPointLayout.h>
#include <proxies/hwui/controls/SwitchVoiceGroupButton.h>

namespace DescriptiveLayouts
{

  GlobalEventSinkBroker::GlobalEventSinkBroker()
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

    registerEvent(EventSinks::ToggleVoiceGroupWithParameterSelection, [eb]() {
      auto selected = eb->getSelected();
      if(SwitchVoiceGroupButton::allowToggling(selected, eb))
      {
        auto scope = SwitchVoiceGroupButton::createToggleVoiceGroupWithParameterHighlightScope();
        Application::get().getHWUI()->toggleCurrentVoiceGroupAndUpdateParameterSelection(scope->getTransaction());
      }
    });

    registerEvent(EventSinks::ToggleVoiceGroup, [eb]() { Application::get().getHWUI()->toggleCurrentVoiceGroup(); });

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
    registerEvent(EventSinks::SwitchToVoicesDetail, [hwui] { hwui->setUiModeDetail(UIDetail::Voices); });

    registerEvent(EventSinks::ToggleDirectLoad, [] {
      auto eb = Application::get().getPresetManager()->getEditBuffer();
      Application::get().getSettings()->getSetting<LoadModeSetting>()->cycleForSoundType(eb->getType());
    });

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
      if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
      {
        if(auto presetList = genericLayout->findControlOfType<GenericPresetList>())
        {
          presetList->action();
        }
      }
    });

    registerEvent(EventSinks::OpenMonoParameterScreen, [eb]() {
      auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();

      eb->undoableSelectParameter({ 364, vg });
    });

    registerEvent(EventSinks::OpenPartScreen, [hwui, eb]() {
      if(eb->getType() != SoundType::Split)
        eb->undoableSelectParameter({ 358, Application::get().getHWUI()->getCurrentVoiceGroup() });
      else
        eb->undoableSelectParameter({ 356, VoiceGroup::Global });
    });

    registerEvent(EventSinks::OpenMasterParameter, [eb] { eb->undoableSelectParameter({ 247, VoiceGroup::Global }); });

    registerEvent(EventSinks::InitSound, [eb, hwui] {
      auto scope = eb->getParent()->getUndoScope().startTransaction("Init Sound");
      eb->undoableInitSound(scope->getTransaction());
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
    });

    registerEvent(EventSinks::OpenUnisonParameter, [hwui, eb]() {
      auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
      eb->undoableSelectParameter({ 249, vg });
    });
  }

  void GlobalEventSinkBroker::registerEvent(EventSinks sink, tAction action)
  {
    m_map[sink] = std::move(action);
  }

  void GlobalEventSinkBroker::fire(EventSinks s)
  {
    try
    {
      m_map.at(s)();
    }
    catch(...)
    {
      nltools::Log::error("EventSink not found in current Layout!", toString(s));
    }
  }
}
