#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <groups/MacroControlMappingGroup.h>
#include <groups/MacroControlsGroup.h>
#include <groups/ScaleGroup.h>
#include <http/UndoScope.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <parameters/MacroControlParameter.h>
#include <parameters/ModulateableParameter.h>
#include <parameters/ModulationRoutingParameter.h>
#include <parameters/PhysicalControlParameter.h>
#include <parameters/ScaleParameter.h>
#include <playground.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <device-settings/SignalFlowIndicationSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/Setting.h>
#include <proxies/hwui/TwoStateLED.h>
#include <tools/StateMachine.h>
#include <proxies/hwui/HWUIEnums.h>
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <device-settings/DebugLevel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetManagerLayout.h>
#include <parameters/names/ParameterDB.h>
#include "PanelUnitParameterEditMode.h"
#include <device-settings/LayoutMode.h>
#include <proxies/hwui/descriptive-layouts/GenericLayout.h>
#include <parameters/PedalParameter.h>

class ParameterInfoLayout;

class ParameterLayout2;

const constexpr auto CombFilterAB = 113;
const constexpr auto CombFilterPM = 133;
const constexpr auto CombFilterPMAB = 135;
const constexpr auto effectsID = 160;
const constexpr auto SVFilterAB = 136;
const constexpr auto SVFilterFM = 153;
const constexpr auto SVFilterFMAB = 155;

PanelUnitParameterEditMode::PanelUnitParameterEditMode()
{
  Application::get().getSettings()->getSetting<SignalFlowIndicationSetting>()->onChange(
      sigc::hide(sigc::mem_fun(this, &PanelUnitParameterEditMode::bruteForceUpdateLeds)));

  Application::get().getPresetManager()->getEditBuffer()->onPresetLoaded(
      sigc::mem_fun(this, &PanelUnitParameterEditMode::bruteForceUpdateLeds));
}

PanelUnitParameterEditMode::~PanelUnitParameterEditMode()
{
}

ButtonParameterMapping &PanelUnitParameterEditMode::getMappings()
{
  return m_mappings;
}

void PanelUnitParameterEditMode::setupFocusAndMode(FocusAndMode focusAndMode)
{
  super::setupFocusAndMode(focusAndMode);
  bruteForceUpdateLeds();
}

static EditPanel &getEditPanel()
{
  auto hwui = Application::get().getHWUI();
  auto &panelUnit = hwui->getPanelUnit();
  return panelUnit.getEditPanel();
}

void PanelUnitParameterEditMode::setup()
{
  m_mappings.forEachButton([=](Buttons buttonID, std::list<int> parameterIDs) {
    std::vector<int> para{ parameterIDs.begin(), parameterIDs.end() };

    if(buttonID != Buttons::BUTTON_75 && buttonID != Buttons::BUTTON_79 && buttonID != Buttons::BUTTON_83
       && buttonID != Buttons::BUTTON_87 && buttonID != Buttons::BUTTON_91 && buttonID != Buttons::BUTTON_95)
      setupButtonConnection(buttonID, createParameterSelectAction(para));
  });

  setupButtonConnection(Buttons::BUTTON_75,
                        bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 243));
  FOR_TESTS(assignedAudioIDs.insert(243));

  setupButtonConnection(Buttons::BUTTON_79,
                        bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 244));
  FOR_TESTS(assignedAudioIDs.insert(244));

  setupButtonConnection(Buttons::BUTTON_83,
                        bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 245));
  FOR_TESTS(assignedAudioIDs.insert(245));

  setupButtonConnection(Buttons::BUTTON_87,
                        bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 246));

  FOR_TESTS(assignedAudioIDs.insert(246));

  setupButtonConnection(Buttons::BUTTON_91,
                        bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 369));

  FOR_TESTS(assignedAudioIDs.insert(369));

  setupButtonConnection(Buttons::BUTTON_95,
                        bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 371));

  FOR_TESTS(assignedAudioIDs.insert(371));

  setupButtonConnection(Buttons::BUTTON_UNDO, [&](Buttons button, ButtonModifiers modifiers, bool state) {
    getEditPanel().getUndoStateMachine().traverse(state ? UNDO_PRESSED : UNDO_RELEASED);
    return false;
  });

  setupButtonConnection(Buttons::BUTTON_REDO, [&](Buttons button, ButtonModifiers modifiers, bool state) {
    getEditPanel().getUndoStateMachine().traverse(state ? REDO_PRESSED : REDO_RELEASED);
    return false;
  });

  setupButtonConnection(Buttons::BUTTON_SOUND, [&](Buttons button, ButtonModifiers modifiers, bool state) {
    if(state)
    {
      auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
      if(focusAndMode.focus == UIFocus::Sound)
        if(focusAndMode.mode == UIMode::Edit)
          Application::get().getHWUI()->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
        else
          Application::get().getHWUI()->setFocusAndMode({ UIFocus::Parameters, UIMode::Select });
      else
        Application::get().getHWUI()->undoableSetFocusAndMode(UIFocus::Sound);
    }

    return true;
  });

  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::mem_fun(this, &PanelUnitParameterEditMode::onParamSelectionChanged));

  Glib::MainContext::get_default()->signal_idle().connect_once([=]() {
    auto hwui = Application::get().getHWUI();
    auto &panelUnit = hwui->getPanelUnit();

    if(panelUnit.getUsageMode().get() == this)
    {
      panelUnit.getEditPanel().getBoled().setupFocusAndMode(hwui->getFocusAndMode());
      bruteForceUpdateLeds();
    }
  });
}

bool PanelUnitParameterEditMode::handleMacroControlButton(bool state, int mcParamId)
{
  auto &mcStateMachine = getMacroControlAssignmentStateMachine();
  mcStateMachine.setCurrentMCParameter(mcParamId);

  bool isAlreadySelected
      = Application::get().getPresetManager()->getEditBuffer()->getSelected()->getID().getNumber() == mcParamId
      && Application::get().getHWUI()->getFocusAndMode().focus == UIFocus::Parameters;

  if(state)
    if(mcStateMachine.traverse(isAlreadySelected ? MacroControlAssignmentEvents::MCPressedWhileSelected
                                                 : MacroControlAssignmentEvents::MCPressedWhileUnselected))
      return true;

  if(!state)
    if(mcStateMachine.traverse(MacroControlAssignmentEvents::MCReleased))
      return true;

  return true;
}

void PanelUnitParameterEditMode::onParamSelectionChanged(Parameter *oldParam, Parameter *newParam)
{
  if(auto mc = dynamic_cast<MacroControlParameter *>(oldParam))
  {
    if(auto ph = dynamic_cast<PhysicalControlParameter *>(newParam))
    {
      if(auto mcm = dynamic_cast<MacroControlMappingGroup *>(
             Application::get().getPresetManager()->getEditBuffer()->getParameterGroupByID(
                 { "MCM", VoiceGroup::Global })))
      {
        if(auto router = mcm->getModulationRoutingParameterFor(ph, mc))
        {
          ph->setUiSelectedModulationRouter(router->getID());
        }
      }
    }
  }
}

Buttons PanelUnitParameterEditMode::findButtonForParameter(Parameter *param) const
{
  int parameterID = param->getID().getNumber();

  if(dynamic_cast<ScaleParameter *>(param))
    parameterID = ScaleGroup::getScaleBaseParameterNumber();

  return m_mappings.findButton(parameterID);
}

std::list<int> PanelUnitParameterEditMode::getButtonAssignments(Buttons button) const
{
  return m_mappings.findParameters(button);
}

UsageMode::tAction PanelUnitParameterEditMode::createParameterSelectAction(std::vector<gint32> toggleAudioIDs)
{
#warning "TODO"
#if _TESTS && 0

  for(gint32 id : toggleAudioIDs)
  {
    if(!Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id)
       && !Application::get().getPresetManager()->getEditBuffer()->findGlobalParameterByID(id))
      g_error("Attempt to link a button to parameter ID %d, which does not exist!", id);

    g_assert(assignedAudioIDs.find(id) == assignedAudioIDs.end());
    assignedAudioIDs.insert(id);
  }

#endif

  return std::bind(&PanelUnitParameterEditMode::toggleParameterSelection, this, toggleAudioIDs, std::placeholders::_3);
}

bool PanelUnitParameterEditMode::toggleParameterSelection(const std::vector<gint32> ids, bool state)
{
  auto voiceGroup = Application::get().getHWUI()->getCurrentVoiceGroup();
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto firstParameterInList = editBuffer->findParameterByID({ ids.front(), voiceGroup });

  auto &mcStateMachine = getMacroControlAssignmentStateMachine();

  if(auto modParam = dynamic_cast<ModulateableParameter *>(firstParameterInList))
  {
    mcStateMachine.setCurrentModulateableParameter({ ids.front(), voiceGroup });

    if(mcStateMachine.traverse(state ? MacroControlAssignmentEvents::ModParamPressed
                                     : MacroControlAssignmentEvents::ModParamReleased))
      return true;
  }

  if(mcStateMachine.getState() != MacroControlAssignmentStates::Initial)
    return true;

  if(state)
  {
    Parameter *selParam = editBuffer->getSelected();

    if(tryParameterToggleOnMacroControl(ids, selParam))
      return true;

    if(isShowingParameterScreen())
    {
      auto selParamID = selParam->getID();
      auto pos = find(ids.begin(), ids.end(), selParamID.getNumber());

      if(pos != ids.end())
        if(switchToNormalModeInCurrentParameterLayout())
          return true;

      for(auto it = ids.begin(); it != ids.end(); ++it)
      {
        if(*it == selParamID.getNumber())
        {
          auto next = std::next(it);

          if(next == ids.end())
            next = ids.begin();

          setParameterSelection({ *next, selParamID.getVoiceGroup() }, state);
          return true;
        }
      }

      if(ParameterId::isGlobal(ids.front()))
        setParameterSelection({ ids.front(), VoiceGroup::Global }, state);
      else
        setParameterSelection({ ids.front(), Application::get().getHWUI()->getCurrentVoiceGroup() }, state);
    }
    else
    {
      if(Application::get().getHWUI()->getButtonModifiers()[SHIFT])
      {
        for(auto paramId : ids)
        {
          auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
          if(ParameterId::isGlobal(paramId))
            vg = VoiceGroup::Global;

          auto param = editBuffer->findParameterByID({ paramId, vg });
          if(param->isChangedFromLoaded())
          {
            setParameterSelection({ paramId, vg }, state);
            return true;
          }
        }
      }

      auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
      setParameterSelection({ ids.front(), vg }, state);
    }
  }

  return true;
}

bool PanelUnitParameterEditMode::switchToNormalModeInCurrentParameterLayout()
{
  auto layout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getLayout();

  if(auto p = std::dynamic_pointer_cast<ModulateableParameterLayout2>(layout))
  {
    return p->switchToNormalMode();
  }
  return false;
}

bool PanelUnitParameterEditMode::tryParameterToggleOnMacroControl(std::vector<gint32> ids, Parameter *selParam)
{
  if(auto mc = dynamic_cast<MacroControlParameter *>(selParam))
  {
    std::list<ParameterId> a;
    for(auto x : ids)
    {
      auto mcVg = mc->getVoiceGroup();
      a.push_back({ x, mcVg });
    }

    if(mc->isSourceOfTargetIn(a))
    {
      for(auto targetId : a)
      {
        if(mc->isSourceOf(targetId))
        {
          setParameterSelection(targetId, true);
          return true;
        }
      }
    }
  }

  return false;
}

bool PanelUnitParameterEditMode::setParameterSelection(const ParameterId &audioID, bool state)
{
  DebugLevel::gassy("setParameterSelection for audioID", audioID);

  if(getMacroControlAssignmentStateMachine().getState() != MacroControlAssignmentStates::Initial)
    return true;

  if(state)
  {
    DebugLevel::gassy("setParameterSelection - state == true");

    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

    if(auto p = editBuffer->findParameterByID(audioID))
    {
      DebugLevel::gassy("selecting param");
      editBuffer->undoableSelectParameter(p);
    }
  }

  return true;
}

bool PanelUnitParameterEditMode::isShowingParameterScreen() const
{
  auto settingValue = Application::get().getSettings()->getSetting<LayoutMode>()->get();
  auto currentLayout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getLayout().get();

  if(settingValue == LayoutVersionMode::Old)
  {
    if(dynamic_cast<ParameterLayout2 *>(currentLayout))
      return true;

    if(dynamic_cast<ParameterInfoLayout *>(currentLayout))
      return true;
  }
  else if(settingValue == LayoutVersionMode::New || settingValue == LayoutVersionMode::Mixed)
  {
    if(auto genericLayout = dynamic_cast<DescriptiveLayouts::GenericLayout *>(currentLayout))
    {
      auto &prototype = genericLayout->getPrototype();
      if(prototype.getDesiredFocusAndMode().focus == UIFocus::Parameters)
      {
        return prototype.getDesiredFocusAndMode().mode == UIMode::Select;
      }
    }
    else
    {
      if(dynamic_cast<ParameterLayout2 *>(currentLayout))
        return true;

      if(dynamic_cast<ParameterInfoLayout *>(currentLayout))
        return true;
    }
  }
  return false;
}

void PanelUnitParameterEditMode::bruteForceUpdateLeds()
{
  m_connectionToMacroControl.disconnect();

  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

  if(Parameter *selParam = editBuffer->getSelected())
  {
    auto selParamID = selParam->getID();

    tLedStates states;
    states.fill(false);

    collectLedStates(states, selParamID);

    if(auto p = dynamic_cast<PhysicalControlParameter *>(selParam))
    {
      auto selModRouter = p->getUiSelectedModulationRouter();

      if(auto router = dynamic_cast<ModulationRoutingParameter *>(editBuffer->findParameterByID(selModRouter)))
      {
        collectLedStates(states, router->getTargetParameter()->getID());
      }
    }

    if(auto router = dynamic_cast<ModulationRoutingParameter *>(selParam))
    {
      if(auto tgt = router->getTargetParameter())
        collectLedStates(states, tgt->getID());
    }

    setLedStates(states);

    if(selParam->getParentGroup()->getID().getName() == "MCs")
    {
      letMacroControlTargetsBlink();
    }

    if(Application::get().getSettings()->getSetting<SignalFlowIndicationSetting>()->get())
      letTargetsBlink(selParam);
  }
}

void PanelUnitParameterEditMode::letTargetsBlink(Parameter *selParam)
{
  auto group = selParam->getParentGroup();
  auto groupName = group->getID().getName();

  if(groupName == "Env A")
  {
    letOtherTargetsBlink({ 62, 73, 96 });
  }
  else if(groupName == "Env B")
  {
    letOtherTargetsBlink({ 66, 92, 103 });
  }
  else if(groupName == "Env C")
  {
    letOtherTargetsBlink({ 56, 59, 70, 80, 86, 89, 100, 110, 118, 126, 132, 143, 147 });
  }
  else if(groupName == "Osc A" || groupName == "Sh A")
  {
    letOscAShaperABlink({ 94, 111, 113, 133, 136, 153, 169 });
  }
  else if(groupName == "Osc B" || groupName == "Sh B")
  {
    letOscBShaperBBlink({ 64, 81, 113, 133, 136, 153, 172 });
  }
  else if(groupName == "Comb")
  {
    letOtherTargetsBlink({ 138, 175, 156 });
  }
  else if(groupName == "FB")
  {
    letOtherTargetsBlink({ 68, 78, 98, 108 });
  }
  else if(groupName == "SVF")
  {
    letOtherTargetsBlink({ 178, 158 });
  }
  else if(groupName == "Gap Filt" || groupName == "Cab" || groupName == "Echo" || groupName == "Reverb"
          || groupName == "Flang")
  {
    letOtherTargetsBlink({ 160 });

    if(groupName == "Reverb")
    {
      letReverbBlink({ 162 });
    }
  }
}

bool isScaleParameter(const ParameterId &paramID)
{
  auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
  return dynamic_cast<ScaleParameter *>(
             Application::get().getPresetManager()->getEditBuffer()->findParameterByID(paramID))
      != nullptr;
}

void PanelUnitParameterEditMode::collectLedStates(tLedStates &states, ParameterId selectedParameterID)
{
  if(isScaleParameter(selectedParameterID))
  {
    selectedParameterID = ParameterId(ScaleGroup::getScaleBaseParameterNumber(), selectedParameterID.getVoiceGroup());
  }

  auto button = m_mappings.findButton(selectedParameterID.getNumber());

  if(button != Buttons::INVALID)
    states[(int) button] = true;
}

const BOLED &PanelUnitParameterEditMode::getBoled() const
{
  return Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
}

BOLED &PanelUnitParameterEditMode::getBoled()
{
  return Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
}

void PanelUnitParameterEditMode::setLedStates(const tLedStates &states)
{
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  for(int i = 0; i < NUM_LEDS; i++)
    panelUnit.getLED((Buttons) i)->setState(states[i] ? TwoStateLED::ON : TwoStateLED::OFF);
}

void PanelUnitParameterEditMode::letMacroControlTargetsBlink()
{
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  Parameter *selParam = editBuffer->getSelected();
  auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();

  if(auto mc = dynamic_cast<MacroControlParameter *>(selParam))
  {
    for(auto t : mc->getTargets())
    {
      if(t->getID().getVoiceGroup() == currentVG)
      {
        auto buttonID = m_mappings.findButton(t->getID().getNumber());
        if(buttonID != Buttons::INVALID)
          panelUnit.getLED(buttonID)->setState(TwoStateLED::BLINK);
      }
    }

    m_connectionToMacroControl
        = mc->onTargetListChanged(mem_fun(this, &PanelUnitParameterEditMode::bruteForceUpdateLeds));
  }
}

void PanelUnitParameterEditMode::letOtherTargetsBlink(const std::vector<int> &targets)
{
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  for(auto targetID : targets)
  {
    auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
    const auto currentParam = editBuffer->findParameterByID({ targetID, vg });

    if(isSignalFlowingThrough(currentParam))
    {
      auto state = editBuffer->getSelected() == currentParam ? TwoStateLED::ON : TwoStateLED::BLINK;
      panelUnit.getLED(m_mappings.findButton(targetID))->setState(state);
    }
  }
}

bool PanelUnitParameterEditMode::isSignalFlowingThrough(const Parameter *p) const
{
  return std::abs(p->getControlPositionValue()) > std::numeric_limits<tControlPositionValue>::epsilon();
}

void PanelUnitParameterEditMode::letOscAShaperABlink(const std::vector<int> &targets)
{
  auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  const auto stateVariableFilterFMAB = editBuffer->findParameterByID({ SVFilterFMAB, vg });
  const auto combFilterPMAB = editBuffer->findParameterByID({ CombFilterPMAB, vg });
  constexpr const auto IdCombMix = 138;
  const auto SVCombMix = editBuffer->findParameterByID({ IdCombMix, vg });
  const auto combMax = SVCombMix->getValue().getUpperBorder();
  const auto combMin = SVCombMix->getValue().getLowerBorder();

  for(auto targetID : targets)
  {
    const auto currentParam = editBuffer->findParameterByID({ targetID, vg });
    switch(targetID)
    {
      case SVFilterAB:
        if(SVCombMix->getControlPositionValue() != combMin && SVCombMix->getControlPositionValue() != combMax)
          if(currentParam->getControlPositionValue() < 1)
            panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
      case CombFilterAB:
        if(currentParam->getControlPositionValue() < 1)
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
      case CombFilterPM:
        if(isSignalFlowingThrough(currentParam) && combFilterPMAB->getControlPositionValue() < 1)
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
      case SVFilterFM:
        if(isSignalFlowingThrough(currentParam) && stateVariableFilterFMAB->getControlPositionValue() < 1)
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
      default:
        if(isSignalFlowingThrough(currentParam))
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
    }
  }
}

void PanelUnitParameterEditMode::letOscBShaperBBlink(const std::vector<int> &targets)
{
  auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  const auto combFilterPMAB = editBuffer->findParameterByID({ CombFilterPMAB, vg });
  const auto stateVariableFilterFMAB = editBuffer->findParameterByID({ SVFilterFMAB, vg });
  constexpr const auto IdCombMix = 138;
  const auto SVCombMix = editBuffer->findParameterByID({ IdCombMix, vg });
  const auto combMax = SVCombMix->getValue().getUpperBorder();
  const auto combMin = SVCombMix->getValue().getLowerBorder();

  for(auto targetID : targets)
  {
    auto currentParam = editBuffer->findParameterByID({ targetID, vg });
    switch(targetID)
    {
      case SVFilterAB:
        if(SVCombMix->getControlPositionValue() != combMin && SVCombMix->getControlPositionValue() != combMax)
          if(currentParam->getControlPositionValue() > 0)
            panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
      case CombFilterAB:
        if(currentParam->getControlPositionValue() > 0)
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
      case CombFilterPM:
        if(isSignalFlowingThrough(currentParam) && combFilterPMAB->getControlPositionValue() > 0)
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
      case SVFilterFM:
        if(isSignalFlowingThrough(currentParam) && stateVariableFilterFMAB->getControlPositionValue() > 0)
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
      default:
        if(isSignalFlowingThrough(currentParam))
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        break;
    }
  }
}

void PanelUnitParameterEditMode::letReverbBlink(const std::vector<int> &targets)
{
  auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  for(auto targetID : targets)
  {
    const auto currentParam = editBuffer->findParameterByID({ targetID, vg });
    const auto effectParam = editBuffer->findParameterByID({ effectsID, vg });

    if(isSignalFlowingThrough(currentParam) && isSignalFlowingThrough(effectParam))
      panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
  }
}

MacroControlAssignmentStateMachine &PanelUnitParameterEditMode::getMacroControlAssignmentStateMachine()
{
  return Application::get().getHWUI()->getPanelUnit().getMacroControlAssignmentStateMachine();
}
