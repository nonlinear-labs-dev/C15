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
#include <proxies/hwui/panel-unit/PanelUnitParameterEditMode.h>
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

void PanelUnitParameterEditMode::assertAllButtonsAssigned()
{
#if _TESTS
  if (Application::get ().getPresetManager ()->getEditBuffer ()->countParameters () != assignedAudioIDs.size ())
  {
    int lastOne = -1;
    for (int id : assignedAudioIDs)
    {
      int expected = lastOne + 1;
      if (expected != 122) // unused param
      {
        if (id != expected)
        {
          auto param = Application::get ().getPresetManager ()->getEditBuffer ()->findParameterByID (expected);
          if (param->getLongName ().find ("Pedal") != 0)
          g_assert(false);
        }
      }
      lastOne = id;
      auto lastParam = Application::get ().getPresetManager ()->getEditBuffer ()->findParameterByID (lastOne);
      if (dynamic_cast<ModulateableParameter*> (lastParam))
      lastOne++;
    }
  }
#endif
}

static EditPanel& getEditPanel()
{
  auto hwui = Application::get().getHWUI();
  auto& panelUnit = hwui->getPanelUnit();
  return panelUnit.getEditPanel();
}

void PanelUnitParameterEditMode::setup()
{
  m_mappings.forEachButton([=](Buttons buttonID, std::list<int> parameterIDs)
  {
    std::vector<int> para
    { parameterIDs.begin(), parameterIDs.end()};

    if(buttonID != Buttons::BUTTON_75 && buttonID != Buttons::BUTTON_79 && buttonID != Buttons::BUTTON_83 && buttonID != Buttons::BUTTON_87)
    setupButtonConnection (buttonID, createParameterSelectAction (para));
  });

  setupButtonConnection(Buttons::BUTTON_75, bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 243));
  FOR_TESTS(assignedAudioIDs.insert(243));

  setupButtonConnection(Buttons::BUTTON_79, bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 244));
  FOR_TESTS(assignedAudioIDs.insert(244));

  setupButtonConnection(Buttons::BUTTON_83, bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 245));
  FOR_TESTS(assignedAudioIDs.insert(245));

  setupButtonConnection(Buttons::BUTTON_87, bind(&PanelUnitParameterEditMode::handleMacroControlButton, this, std::placeholders::_3, 246));
  FOR_TESTS(assignedAudioIDs.insert(246));

  assertAllButtonsAssigned();

  setupButtonConnection(Buttons::BUTTON_UNDO, [&] (Buttons button, ButtonModifiers modifiers, bool state)
  {
    getEditPanel().getUndoStateMachine().traverse(state ? UNDO_PRESSED : UNDO_RELEASED);
    return false;
  });

  setupButtonConnection(Buttons::BUTTON_REDO, [&] (Buttons button, ButtonModifiers modifiers, bool state)
  {
    getEditPanel().getUndoStateMachine().traverse(state ? REDO_PRESSED : REDO_RELEASED);
    return false;
  });

  setupButtonConnection(Buttons::BUTTON_SOUND, [&] (Buttons button, ButtonModifiers modifiers, bool state)
  {
    if(state)
    {
      if(Application::get().getHWUI()->getFocusAndMode().focus == UIFocus::Sound)
      Application::get ().getHWUI ()->setFocusAndMode (
          { UIFocus::Parameters, UIMode::Select});
      else
      Application::get().getHWUI()->undoableSetFocusAndMode(UIFocus::Sound);
    }

    return true;
  });

  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::mem_fun(this, &PanelUnitParameterEditMode::onParamSelectionChanged));

  Glib::MainContext::get_default()->signal_idle().connect_once([=]()
  {
    auto hwui = Application::get ().getHWUI ();
    auto &panelUnit = hwui->getPanelUnit ();

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

  bool isAlreadySelected = Application::get().getPresetManager()->getEditBuffer()->getSelected()->getID() == mcParamId &&
					Application::get().getHWUI()->getFocusAndMode().focus == UIFocus::Parameters;

  if(state)
    if(mcStateMachine.traverse(isAlreadySelected ? MacroControlAssignmentEvents::MCPressedWhileSelected : MacroControlAssignmentEvents::MCPressedWhileUnselected))
      return true;

  if(!state)
    if(mcStateMachine.traverse(MacroControlAssignmentEvents::MCReleased))
      return true;

  return true;
}

void PanelUnitParameterEditMode::onParamSelectionChanged(Parameter * oldParam, Parameter * newParam)
{
  if(auto mc = dynamic_cast<MacroControlParameter*>(oldParam))
  {
    if(auto ph = dynamic_cast<PhysicalControlParameter*>(newParam))
    {
      if(auto mcm = dynamic_cast<MacroControlMappingGroup*>(Application::get().getPresetManager()->getEditBuffer()->getParameterGroupByID(
          "MCM")))
      {
        if(auto router = mcm->getModulationRoutingParameterFor(ph, mc))
        {
          ph->setUiSelectedModulationRouter(router->getID());
        }
      }
    }
  }
}

Buttons PanelUnitParameterEditMode::findButtonForParameter(Parameter * param) const
{
  int parameterID = param->getID();

  if(dynamic_cast<ScaleParameter*>(param))
    parameterID = ScaleGroup::getScaleBaseParameterID();

  return m_mappings.findButton(parameterID);
}

std::list<int> PanelUnitParameterEditMode::getButtonAssignments(Buttons button) const
{
  return m_mappings.findParameters(button);
}

UsageMode::tAction PanelUnitParameterEditMode::createParameterSelectAction(vector<gint32> toggleAudioIDs)
{
#if _TESTS

  for (gint32 id : toggleAudioIDs)
  {
    if (!Application::get ().getPresetManager()->getEditBuffer()->findParameterByID (id))
    g_error ("Attempt to link a button to parameter ID %d, which does not exist!", id);

    g_assert (assignedAudioIDs.find (id) == assignedAudioIDs.end());
    assignedAudioIDs.insert (id);
  }

#endif

  return bind(&PanelUnitParameterEditMode::toggleParameterSelection, this, toggleAudioIDs, std::placeholders::_3);
}

UsageMode::tAction PanelUnitParameterEditMode::createParameterSelectAction(gint32 audioID)
{
  return bind(&PanelUnitParameterEditMode::toggleParameterSelection, this, vector<gint32>(audioID), std::placeholders::_3);
}

bool PanelUnitParameterEditMode::toggleParameterSelection(vector<gint32> ids, bool state)
{
  shared_ptr<EditBuffer> editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto firstParameterInList = editBuffer->findParameterByID(ids.front());

  auto &mcStateMachine = getMacroControlAssignmentStateMachine();

  if(auto modParam = dynamic_cast<ModulateableParameter*>(firstParameterInList))
  {
    mcStateMachine.setCurrentModulateableParameter(ids.front());

    if(mcStateMachine.traverse(state ? MacroControlAssignmentEvents::ModParamPressed : MacroControlAssignmentEvents::ModParamReleased))
      return true;
  }

  if(mcStateMachine.getState() != MacroControlAssignmentStates::Initial)
    return true;

  if(state)
  {
    Parameter * selParam = editBuffer->getSelected();

    if(tryParameterToggleOnMacroControl(ids, selParam))
      return true;

    if(isShowingParameterScreen())
    {
      auto pos = find(ids.begin(), ids.end(), selParam->getID());

      if(pos != ids.end())
        if(switchToNormalModeInCurrentParameterLayout())
          return true;

      gint32 selParamID = selParam ? selParam->getID() : -1;

      for(auto it = ids.begin(); it != ids.end(); ++it)
      {
        if(*it == selParamID)
        {
          auto next = std::next(it);

          if(next == ids.end())
            next = ids.begin();

          setParameterSelection(*next, state);
          return true;
        }
      }

      setParameterSelection(ids.front(), state);
    }
    else
    {
      setParameterSelection(ids.front(), state);
    }
  }

  return true;
}

bool PanelUnitParameterEditMode::switchToNormalModeInCurrentParameterLayout()
{
  auto layout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getLayout();

  if(auto p = dynamic_pointer_cast<ModulateableParameterLayout2>(layout))
  {
    return p->switchToNormalMode();
  }
  return false;
}

bool PanelUnitParameterEditMode::tryParameterToggleOnMacroControl(vector<gint32> ids, Parameter * selParam)
{
  if(auto mc = dynamic_cast<MacroControlParameter *>(selParam))
  {
    list<gint32> a;
    for (auto x : ids)
    {
      a.push_back(x);
    }

    if (mc->isSourceOfTargetIn(a))
    {
      for (gint32 targetId : a)
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

bool PanelUnitParameterEditMode::setParameterSelection(gint32 audioID, bool state)
{
  DebugLevel::gassy("setParameterSelection for audioID", audioID);

  if(getMacroControlAssignmentStateMachine().getState() != MacroControlAssignmentStates::Initial)
    return true;

  if(state)
  {
    DebugLevel::gassy("setParameterSelection - state == true");

    shared_ptr<EditBuffer> editBuffer = Application::get().getPresetManager()->getEditBuffer();

    if(Parameter * p = editBuffer->findParameterByID(audioID))
    {
      DebugLevel::gassy("selecting param");
      auto old = editBuffer->getSelected();
      editBuffer->undoableSelectParameter(p);
    }
  }

  return true;
}

bool PanelUnitParameterEditMode::isShowingParameterScreen() const
{
  auto layout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getLayout();

  if(dynamic_pointer_cast<ParameterLayout2>(layout))
    return true;

  if(dynamic_pointer_cast<ParameterInfoLayout>(layout))
    return true;

  return false;
}

void PanelUnitParameterEditMode::bruteForceUpdateLeds()
{
  m_connectionToMacroControl.disconnect();

  shared_ptr<EditBuffer> editBuffer = Application::get().getPresetManager()->getEditBuffer();

  if(Parameter * selParam = editBuffer->getSelected())
  {
    auto selParamID = selParam->getID();

    tLedStates states;
    states.fill(false);

    collectLedStates(states, selParamID);

    if(auto p = dynamic_cast<PhysicalControlParameter*>(selParam))
    {
      auto selModRouter = p->getUiSelectedModulationRouter();

      if(auto router = dynamic_cast<ModulationRoutingParameter *>(editBuffer->findParameterByID(selModRouter)))
      {
        collectLedStates(states, router->getTargetParameter()->getID());
      }
    }

    if(auto router = dynamic_cast<ModulationRoutingParameter*>(selParam))
    {
      collectLedStates(states, router->getTargetParameter()->getID());
    }

    setLedStates(states);

    if(selParam->getParentGroup()->getID() == "MCs")
    {
      letMacroControlTargetsBlink();
    }

    if(Application::get().getSettings()->getSetting<SignalFlowIndicationSetting>()->get() == true)
      letTargetsBlink(selParam);
  }
}

void PanelUnitParameterEditMode::letTargetsBlink(Parameter* selParam)
{
  auto group = selParam->getParentGroup();

  if(group->getID() == "Env A")
  {
    letOtherTargetsBlink( { 62, 73, 96 });
  }
  else if(group->getID() == "Env B")
  {
    letOtherTargetsBlink( { 66, 92, 103 });
  }
  else if(group->getID() == "Env C")
  {
    letOtherTargetsBlink( { 56, 59, 70, 80, 86, 89, 100, 110, 118, 126, 132, 143, 147 });
  }
  else if(group->getID() == "Osc A" || group->getID() == "Sh A")
  {
    letOscAShaperABlink( { 94, 111, 113, 133, 136, 153, 169 });
  }
  else if(group->getID() == "Osc B" || group->getID() == "Sh B")
  {
    letOscBShaperBBlink( { 64, 81, 113, 133, 136, 153, 172 });
  }
  else if(group->getID() == "Comb")
  {
    letOtherTargetsBlink( { 138, 175, 156 });
  }
  else if(group->getID() == "FB")
  {
    letOtherTargetsBlink( { 68, 78, 98, 108 });
  }
  else if(group->getID() == "SVF")
  {
    letOtherTargetsBlink( { 178, 158 });
  }
  else if(group->getID() == "Flang" || group->getID() == "Gap Filt" || group->getID() == "Cab" || group->getID() == "Echo"
          || group->getID() == "Reverb")
  {
    letOtherTargetsBlink( { 160 });

    if(group->getID() == "Reverb")
    {
      letReverbBlink( { 162 });
    }
  }
}

void PanelUnitParameterEditMode::collectLedStates(tLedStates &states, int selectedParameterID)
{
  auto button = m_mappings.findButton(selectedParameterID);

  if(button != Buttons::INVALID)
    states[(int)button] = true;
}

shared_ptr<Layout> PanelUnitParameterEditMode::getCurrentBoledLayout() const
{
  return getBoled().getLayout();
}

const BOLED &PanelUnitParameterEditMode::getBoled() const
{
  return Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
}

BOLED & PanelUnitParameterEditMode::getBoled()
{
  return Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
}

void PanelUnitParameterEditMode::setLedStates(const tLedStates &states)
{
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  for(int i = 0; i < NUM_LEDS; i++)
    panelUnit.getLED((Buttons)i)->setState(states[i] ? TwoStateLED::ON : TwoStateLED::OFF);
}

void PanelUnitParameterEditMode::letMacroControlTargetsBlink()
{
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();
  shared_ptr<EditBuffer> editBuffer = Application::get().getPresetManager()->getEditBuffer();
  Parameter * selParam = editBuffer->getSelected();

  if(auto mc = dynamic_cast<MacroControlParameter *>(selParam))
  {
    for(auto t : mc->getTargets())
    {
      auto buttonID = m_mappings.findButton(t->getID());
      panelUnit.getLED(buttonID)->setState(TwoStateLED::BLINK);
    }

    m_connectionToMacroControl = mc->onTargetListChanged(mem_fun(this, &PanelUnitParameterEditMode::bruteForceUpdateLeds));
  }
}

void PanelUnitParameterEditMode::letOtherTargetsBlink(const std::vector<int> &targets)
{
  shared_ptr<EditBuffer> editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  for(auto targetID : targets)
  {
    const auto currentParam = editBuffer->findParameterByID(targetID);

    if(currentParam->getControlPositionValue() != currentParam->getDefaultValue())
    {
      panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
    }
  }
}

void PanelUnitParameterEditMode::letOscAShaperABlink(const std::vector<int> &targets)
{
  shared_ptr<EditBuffer> editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  const auto FMAB = editBuffer->findParameterByID(SVFilterFMAB);
  const auto combFilterPMAB = editBuffer->findParameterByID(CombFilterPMAB);

  for(auto targetID : targets)
  {
    const auto currentParam = editBuffer->findParameterByID(targetID);
    switch(targetID)
    {
      case CombFilterAB:
      case SVFilterAB:
        if(currentParam->getControlPositionValue() < 1.d)
        {
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        }
        break;
      case CombFilterPM:
        if(combFilterPMAB->getControlPositionValue() < 1.d && currentParam->getControlPositionValue() != 0.d)
        {
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        }
        break;
      case SVFilterFM:
        if(currentParam->getControlPositionValue() != currentParam->getDefaultValue() && FMAB->getControlPositionValue() < 1.d)
        {
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        }
        break;
      default:
        if(currentParam->getControlPositionValue() != currentParam->getDefaultValue())
        {
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        }
        break;
    }
  }
}

void PanelUnitParameterEditMode::letOscBShaperBBlink(const std::vector<int> &targets)
{
  shared_ptr<EditBuffer> editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  const auto combFilterPMAB = editBuffer->findParameterByID(CombFilterPMAB);
  const auto stateVariableFilterFMAB = editBuffer->findParameterByID(SVFilterFMAB);

  for(auto targetID : targets)
  {
    auto currentParam = editBuffer->findParameterByID(targetID);
    switch(targetID)
    {
      case CombFilterPM:
        if(currentParam->getControlPositionValue() != currentParam->getDefaultValue() && combFilterPMAB->getControlPositionValue()
            != combFilterPMAB->getDefaultValue())
        {
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        }
        break;
      case SVFilterFM:
        if(currentParam->getControlPositionValue() != currentParam->getDefaultValue() && stateVariableFilterFMAB->getControlPositionValue()
            != stateVariableFilterFMAB->getDefaultValue())
        {
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        }
        break;
      default:
        if(currentParam->getControlPositionValue() != currentParam->getDefaultValue())
        {
          panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
        }
        break;
    }
  }
}

void PanelUnitParameterEditMode::letReverbBlink(const std::vector<int> &targets)
{
  shared_ptr<EditBuffer> editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();

  for(auto targetID : targets)
  {
    const auto currentParam = editBuffer->findParameterByID(targetID);
    const auto effectParam = editBuffer->findParameterByID(effectsID);
    if(currentParam->getControlPositionValue() > 0.d && effectParam->getControlPositionValue() != 0.d)
    {
      panelUnit.getLED(m_mappings.findButton(targetID))->setState(TwoStateLED::BLINK);
    }
  }
}

MacroControlAssignmentStateMachine &PanelUnitParameterEditMode::getMacroControlAssignmentStateMachine()
{
  return Application::get().getHWUI()->getPanelUnit().getMacroControlAssignmentStateMachine();
}
