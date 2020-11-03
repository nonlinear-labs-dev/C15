#include <Application.h>
#include <device-settings/EnumSetting.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/panel-unit/PanelUnitParameterEditMode.h>
#include <proxies/hwui/panel-unit/PanelUnitPresetMode.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include <proxies/hwui/TwoStateLED.h>
#include <xml/FileOutStream.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulationRoutingParameter.h>
#include <parameters/PhysicalControlParameter.h>
#include <parameters/MacroControlParameter.h>
#include <groups/MacroControlsGroup.h>
#include <http/UndoScope.h>
#include <nltools/messaging/Message.h>
#include <device-settings/ScreenSaverTimeoutSetting.h>
#include "ScreenSaverUsageMode.h"

PanelUnit::PanelUnit()
{
  const int numLEDs = 96;

  m_leds.reserve(numLEDs);

  for(int i = 0; i < numLEDs; i++)
    m_leds.emplace_back(new TwoStateLED(i));

  m_macroControlAssignmentStateMachine.registerHandler(MacroControlAssignmentStates::Selected, [=]() {
    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
    auto p = editBuffer->getSelected(Application::get().getHWUI()->getCurrentVoiceGroup());

    if(auto mrp = dynamic_cast<ModulationRoutingParameter *>(p))
    {
      mrp->getSourceParameter()->setUiSelectedModulationRouter(p->getID());
    }

    auto currentMc = m_macroControlAssignmentStateMachine.getCurrentMCParameter();
    editBuffer->undoableSelectParameter({ currentMc, VoiceGroup::Global });
    return true;
  });

  m_macroControlAssignmentStateMachine.registerHandler(MacroControlAssignmentStates::Assign, [=]() {
    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
    auto selParam = editBuffer->getSelected(Application::get().getHWUI()->getCurrentVoiceGroup());
    auto mc = MacroControlsGroup::paramIDToModSrc(selParam->getID());

    auto targetId = m_macroControlAssignmentStateMachine.getCurrentModulateableParameter();
    auto target = editBuffer->findParameterByID(targetId);

    if(auto modParam = dynamic_cast<ModulateableParameter *>(target))
    {
      if(modParam->getModulationSource() == mc)
      {
        auto scope = Application::get().getUndoScope()->startTransaction("Remove Modulation Source");
        modParam->undoableSelectModSource(scope->getTransaction(), MacroControls::NONE);
      }
      else
      {
        auto scope = Application::get().getUndoScope()->startTransaction("Set Modulation Source");
        modParam->undoableSelectModSource(scope->getTransaction(), mc);

        editBuffer->undoableSelectParameter(scope->getTransaction(), modParam);

        auto hwui = Application::get().getHWUI();
        auto &boled = hwui->getPanelUnit().getEditPanel().getBoled();
        boled.onLayoutInstalled([&](Layout *l) {
          if(auto modParamLayout = dynamic_cast<ModulateableParameterSelectLayout2 *>(l))
          {
            modParamLayout->installMcAmountScreen();
            m_macroControlAssignmentStateMachine.setState(MacroControlAssignmentStates::Initial);
          }
        });
      }
    }
    return true;
  });

  m_macroControlAssignmentStateMachine.registerHandler(MacroControlAssignmentStates::SelectSource, [=]() {
    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
    auto p = editBuffer->getSelected(Application::get().getHWUI()->getCurrentVoiceGroup());
    auto currentSource = choseHWBestSourceForMC(p->getID());
    editBuffer->undoableSelectParameter(currentSource);
    m_macroControlAssignmentStateMachine.setState(MacroControlAssignmentStates::Initial);
    return true;
  });

  Application::get().getSettings()->getSetting<ScreenSaverTimeoutSetting>()->onScreenSaverStateChanged(
      sigc::mem_fun(this, &PanelUnit::onScreenSaverStateChanged));

  nltools::msg::onConnectionEstablished(nltools::msg::EndPoint::PanelLed,
                                        sigc::mem_fun(this, &PanelUnit::onBBBBConnected));
}

PanelUnit::~PanelUnit() = default;

ParameterId PanelUnit::choseHWBestSourceForMC(const ParameterId &mcParamId) const
{
  if(auto mc = dynamic_cast<MacroControlParameter *>(
         Application::get().getPresetManager()->getEditBuffer()->findParameterByID(mcParamId)))
  {
    return mc->getUiSelectedHardwareSource();
  }

  return mcParamId;
}

void PanelUnit::onScreenSaverStateChanged(bool state)
{
  if(state)
  {
    m_stashedUsageMode = getUsageMode();
    setUsageMode(new ScreenSaverUsageMode());
  }
  else if(m_stashedUsageMode)
  {
    restoreUsageMode(m_stashedUsageMode);
    m_stashedUsageMode = nullptr;
  }
}

void PanelUnit::init()
{
  setUsageMode(new PanelUnitParameterEditMode());

  for(auto &led : m_leds)
    led->init();

  m_editPanel.init();
}

void PanelUnit::onBBBBConnected()
{
  for(auto &led : m_leds)
    led->syncBBBB();
}

void PanelUnit::turnLedsOff()
{
  for(auto &led : m_leds)
    led->setState(TwoStateLED::OFF);
}

void PanelUnit::setupFocusAndMode()
{
  setupFocusAndMode(Application::get().getHWUI()->getFocusAndMode());
}

void PanelUnit::setupFocusAndMode(FocusAndMode focusAndMode)
{
  installUsageMode(focusAndMode);

  m_editPanel.setupFocusAndMode(focusAndMode);
  getUsageMode()->setupFocusAndMode(focusAndMode);
}

void PanelUnit::installUsageMode(FocusAndMode focusAndMode)
{
  switch(focusAndMode.focus)
  {
    case UIFocus::Presets:
    case UIFocus::Banks:
      if(typeid(PanelUnitPresetMode) != typeid(*getUsageMode().get()))
        setUsageMode(new PanelUnitPresetMode());
      break;

    case UIFocus::Sound:
      if(typeid(PanelUnitSoundMode) != typeid(*getUsageMode().get()))
        setUsageMode(new PanelUnitSoundMode());
      break;

    case UIFocus::Parameters:
    case UIFocus::Setup:
    case UIFocus::Unchanged:
      if(typeid(PanelUnitParameterEditMode) != typeid(*getUsageMode().get()))
        setUsageMode(new PanelUnitParameterEditMode());
      break;
  }
}

PanelUnit::tLed PanelUnit::getLED(Buttons id)
{
  return m_leds[(int) id];
}

void PanelUnit::onTimeout()
{
  m_editPanel.onTimeout();
}

EditPanel &PanelUnit::getEditPanel()
{
  return m_editPanel;
}

const EditPanel &PanelUnit::getEditPanel() const
{
  return m_editPanel;
}

bool PanelUnit::onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state)
{
  if(super::onButtonPressed(buttonID, modifiers, state))
  {
    return true;
  }

  return m_editPanel.onButtonPressed(buttonID, modifiers, state);
}

MacroControlAssignmentStateMachine &PanelUnit::getMacroControlAssignmentStateMachine()
{
  return m_macroControlAssignmentStateMachine;
}
