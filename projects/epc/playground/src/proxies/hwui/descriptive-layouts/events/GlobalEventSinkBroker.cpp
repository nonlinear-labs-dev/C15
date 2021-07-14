#include <utility>
#include "GlobalEventSinkBroker.h"
#include <parameters/ModulateableParameter.h>
#include <libundo/undo/Scope.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <device-settings/DirectLoadSetting.h>
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
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableDualVoiceGroupMasterAndSplitPointLayout.h>
#include <proxies/hwui/controls/SwitchVoiceGroupButton.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <parameter_declarations.h>
#include <use-cases/DirectLoadUseCases.h>

namespace DescriptiveLayouts
{

  GlobalEventSinkBroker::GlobalEventSinkBroker()
  {
    auto eb = Application::get().getPresetManager()->getEditBuffer();
    auto hwui = Application::get().getHWUI();

    registerEvent(EventSinks::Swallow, []() { return; });

    registerEvent(EventSinks::IncParam, [eb, hwui]() {
      if(auto p = eb->getSelected(hwui->getCurrentVoiceGroup()))
      {
        auto modifiers = hwui->getButtonModifiers();
        ParameterUseCases useCase(p);
        useCase.incDec(1, modifiers[ButtonModifier::FINE], modifiers[ButtonModifier::SHIFT]);
      }
    });

    registerEvent(EventSinks::DecParam, [eb, hwui]() {
      if(auto p = eb->getSelected(hwui->getCurrentVoiceGroup()))
      {
        auto modifiers = hwui->getButtonModifiers();
        ParameterUseCases useCase(p);
        useCase.incDec(-1, modifiers[ButtonModifier::FINE], modifiers[ButtonModifier::SHIFT]);
      }
    });

    registerEvent(EventSinks::IncMCSel, [eb, hwui]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())))
      {
        ModParameterUseCases useCase(modParam);
        useCase.incMCSelection(1);
      }
    });

    registerEvent(EventSinks::DecMCSel, [eb, hwui]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())))
      {
        ModParameterUseCases useCase(modParam);
        useCase.incMCSelection(-1);
      }
    });

    registerEvent(EventSinks::IncMCAmt, [eb, hwui]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())))
      {
        ModParameterUseCases useCase(modParam);
        useCase.incModAmount(1, false);
      }
    });

    registerEvent(EventSinks::DecMCAmt, [eb, hwui]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())))
      {
        ModParameterUseCases useCase(modParam);
        useCase.incModAmount(-1, false);
      }
    });

    registerEvent(EventSinks::IncMCPos, [eb, hwui]() {
      if(auto modP = dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())))
      {
        if(auto mc = modP->getMacroControl())
        {
          auto hwuiModifiers = hwui->getButtonModifiers();
          auto fine = hwuiModifiers[ButtonModifier::FINE];
          auto shift = hwuiModifiers[ButtonModifier::SHIFT];
          MacroControlParameterUseCases useCase(mc);
          useCase.incDecPosition(1, fine, shift);
        }
      }
    });

    registerEvent(EventSinks::DecMCPos, [eb, hwui]() {
      if(auto modP = dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())))
      {
        if(auto mc = modP->getMacroControl())
        {
          auto hwuiModifiers = hwui->getButtonModifiers();
          auto fine = hwuiModifiers[ButtonModifier::FINE];
          auto shift = hwuiModifiers[ButtonModifier::SHIFT];
          MacroControlParameterUseCases useCase(mc);
          useCase.incDecPosition(-1, fine, shift);
        }
      }
    });

    registerEvent(EventSinks::ToggleVoiceGroupWithParameterSelection, [hwui]() { hwui->toggleCurrentVoiceGroup(); });

    registerEvent(EventSinks::ToggleVoiceGroup, [hwui]() { hwui->toggleCurrentVoiceGroup(); });

    /*
       * UIFocus
       */
    registerEvent(EventSinks::SwitchToParameterFocus,
                  [hwui] { hwui->undoableSetFocusAndMode(FocusAndMode { UIFocus::Parameters }); });
    registerEvent(EventSinks::SwitchToSoundFocus, [hwui] {
      hwui->undoableSetFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
    });
    registerEvent(EventSinks::SwitchToPresetFocus,
                  [hwui] { hwui->undoableSetFocusAndMode(FocusAndMode { UIFocus::Presets }); });
    registerEvent(EventSinks::SwitchToBankFocus,
                  [hwui] { hwui->undoableSetFocusAndMode(FocusAndMode { UIFocus::Banks }); });
    registerEvent(EventSinks::SwitchToSetupFocus,
                  [hwui] { hwui->undoableSetFocusAndMode(FocusAndMode { UIFocus::Setup }); });

    /*
       * UIMode
       */
    registerEvent(EventSinks::SwitchToEditMode,
                  [hwui]() { hwui->undoableSetFocusAndMode(FocusAndMode { UIMode::Edit }); });
    registerEvent(EventSinks::SwitchToSelectMode,
                  [hwui]() { hwui->undoableSetFocusAndMode(FocusAndMode { UIMode::Select }); });

    /*
       * UIDetail
       */
    registerEvent(EventSinks::SwitchToInitDetail, [hwui]() { hwui->setUiModeDetail(UIDetail::Init); });
    registerEvent(EventSinks::SwitchToButtonADetail, [hwui] { hwui->setUiModeDetail(UIDetail::ButtonA); });
    registerEvent(EventSinks::SwitchToButtonBDetail, [hwui] { hwui->setUiModeDetail(UIDetail::ButtonB); });
    registerEvent(EventSinks::SwitchToButtonCDetail, [hwui] { hwui->setUiModeDetail(UIDetail::ButtonC); });
    registerEvent(EventSinks::SwitchToButtonDDetail, [hwui] { hwui->setUiModeDetail(UIDetail::ButtonD); });
    registerEvent(EventSinks::SwitchToVoicesDetail, [hwui] { hwui->setUiModeDetail(UIDetail::Voices); });

    registerEvent(EventSinks::ToggleDirectLoad, [hwui] {
      DirectLoadUseCases useCase(Application::get().getSettings()->getSetting<DirectLoadSetting>().get());
      useCase.toggleDirectLoadFromHWUI(hwui);
    });

    registerEvent(EventSinks::SwitchToMCAmtDetail, [hwui, eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())))
      {
        if(modParam->getModulationSource() != MacroControls::NONE)
        {
          hwui->setUiModeDetail(UIDetail::MCAmount);
        }
      }
    });

    registerEvent(EventSinks::SwitchToMCModRangeDetail, [hwui, eb]() {
      if(auto modParam = dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())))
      {
        if(modParam->getModulationSource() != MacroControls::NONE)
        {
          hwui->setUiModeDetail(UIDetail::MCModRange);
        }
      }
    });

    registerEvent(EventSinks::SwitchToMCSelectDetail, [hwui, eb]() {
      if(dynamic_cast<ModulateableParameter *>(eb->getSelected(hwui->getCurrentVoiceGroup())) != nullptr)
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
      EditBufferUseCases useCase(eb);
      auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
      if(eb->getType() == SoundType::Split)
        useCase.selectParameter(ParameterId { 364, vg }, true);
      else
        useCase.selectParameter(ParameterId { 364, VoiceGroup::I }, true);
    });

    registerEvent(EventSinks::OpenPartScreen, [eb]() {
      EditBufferUseCases ebUseCases { eb };
      ebUseCases.selectParameter({ 358, Application::get().getHWUI()->getCurrentVoiceGroup() }, true);
    });

    registerEvent(EventSinks::OpenMasterParameter, [eb] {
      EditBufferUseCases ebUseCases { eb };
      ebUseCases.selectParameter({ 247, VoiceGroup::Global }, true);
    });

    registerEvent(EventSinks::InitSound, [eb] {
      SoundUseCases useCases { eb, eb->getParent() };
      useCases.initSound();
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
    });

    registerEvent(EventSinks::OpenUnisonParameter, [eb]() {
      auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
      EditBufferUseCases ebUseCases { eb };
      ebUseCases.selectParameter({ 249, vg }, true);
    });

    registerEvent(EventSinks::IncSplitPoint, [hwui, eb]() {
      EditBufferUseCases ebUseCases(eb);
      auto currentVG = hwui->getCurrentVoiceGroup();
      if(auto parameterUseCases = ebUseCases.getUseCase({ C15::PID::Split_Split_Point, currentVG }))
      {
        parameterUseCases->incDec(1, false, false);
      }
    });

    registerEvent(EventSinks::DecSplitPoint, [hwui, eb]() {
      EditBufferUseCases ebUseCases(eb);
      auto currentVG = hwui->getCurrentVoiceGroup();
      if(auto parameterUseCases = ebUseCases.getUseCase({ C15::PID::Split_Split_Point, currentVG }))
      {
        parameterUseCases->incDec(-1, false, false);
      }
    });

    registerEvent(EventSinks::LayerMuteInc, [eb]() {
      auto muteI = eb->findParameterByID({ 395, VoiceGroup::I });
      auto muteII = eb->findParameterByID({ 395, VoiceGroup::II });
      const auto vgIMuted = muteI->getControlPositionValue() > 0.5;
      const auto vgIIMuted = muteII->getControlPositionValue() > 0.5;
      EditBufferUseCases useCases(eb);

      if(!vgIMuted && !vgIIMuted)
      {
        useCases.mutePart(VoiceGroup::I);
      }
      else if(vgIIMuted)
      {
        useCases.unmuteBothPartsWithTransactionNameForPart(VoiceGroup::II);
      }
    });

    registerEvent(EventSinks::LayerMuteDec, [eb]() {
      auto muteI = eb->findParameterByID({ 395, VoiceGroup::I });
      auto muteII = eb->findParameterByID({ 395, VoiceGroup::II });
      const auto vgIMuted = muteI->getControlPositionValue() > 0.5;
      const auto vgIIMuted = muteII->getControlPositionValue() > 0.5;
      EditBufferUseCases useCases(eb);

      if(!vgIMuted && !vgIIMuted)
      {
        useCases.mutePart(VoiceGroup::II);
      }
      else if(vgIMuted)
      {
        useCases.unmuteBothPartsWithTransactionNameForPart(VoiceGroup::I);
      }
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
