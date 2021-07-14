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

PanelUnit::PanelUnit()
{
  const int numLEDs = 96;

  m_leds.reserve(numLEDs);

  for(int i = 0; i < numLEDs; i++)
    m_leds.emplace_back(new TwoStateLED(i));

  m_macroControlAssignmentStateMachine.registerHandler(MacroControlAssignmentStates::Selected, [=]() {
    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
    EditBufferUseCases ebUseCases { editBuffer };

    auto p = editBuffer->getSelected(Application::get().getHWUI()->getCurrentVoiceGroup());

    if(auto mrp = dynamic_cast<ModulationRoutingParameter *>(p))
    {
      mrp->getSourceParameter()->setUiSelectedModulationRouter(p->getID());
    }

    auto currentMc = m_macroControlAssignmentStateMachine.getCurrentMCParameter();
    ebUseCases.selectParameter({ currentMc, VoiceGroup::Global }, true);
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
      ModParameterUseCases useCase(modParam);
      if(modParam->getModulationSource() == mc)
      {
        useCase.removeModSource();
      }
      else
      {
        auto hwui = Application::get().getHWUI();
        auto &boled = hwui->getPanelUnit().getEditPanel().getBoled();
        m_signalInitializeInstalledLayoutOnce
            = boled.onLayoutInstalled(sigc::mem_fun(this, &PanelUnit::initModulateableParameterLayout));

        useCase.selectModSourceAndSelectTargetParameter(mc);
      }
    }
    return true;
  });

  m_macroControlAssignmentStateMachine.registerHandler(MacroControlAssignmentStates::SelectSource, [=]() {
    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
    EditBufferUseCases ebUseCases { editBuffer };
    auto p = editBuffer->getSelected(Application::get().getHWUI()->getCurrentVoiceGroup());
    auto currentSource = choseHWBestSourceForMC(p->getID());
    ebUseCases.selectParameter(currentSource, true);
    m_macroControlAssignmentStateMachine.setState(MacroControlAssignmentStates::Initial);
    return true;
  });

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
  if(m_overlayUsageMode)
  {
    return m_overlayUsageMode->onButtonPressed(buttonID, modifiers, state);
  }
  else if(super::onButtonPressed(buttonID, modifiers, state))
  {
    return true;
  }

  return m_editPanel.onButtonPressed(buttonID, modifiers, state);
}

MacroControlAssignmentStateMachine &PanelUnit::getMacroControlAssignmentStateMachine()
{
  return m_macroControlAssignmentStateMachine;
}

void PanelUnit::initModulateableParameterLayout(Layout *l)
{
  if(auto modParamLayout = dynamic_cast<ModulateableParameterSelectLayout2 *>(l))
  {
    modParamLayout->installMcAmountScreen();
    m_macroControlAssignmentStateMachine.setState(MacroControlAssignmentStates::Initial);
  }
  m_signalInitializeInstalledLayoutOnce.disconnect();
}

const std::vector<std::shared_ptr<TwoStateLED>> &PanelUnit::getLeds()
{
  return m_leds;
}
