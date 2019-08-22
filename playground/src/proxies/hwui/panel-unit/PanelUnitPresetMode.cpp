#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <parameters/Parameter.h>
#include <parameters/MacroControlParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/panel-unit/PanelUnitPresetMode.h>
#include <proxies/hwui/TwoStateLED.h>
#include <memory>
#include "PanelUnitPresetMode.h"
#include <device-settings/HighlightChangedParametersSetting.h>

PanelUnitPresetMode::PanelUnitPresetMode()
    : m_bruteForceLedThrottler(std::chrono::milliseconds(40))
{
  DebugLevel::gassy(__PRETTY_FUNCTION__);

  Application::get().getHWUI()->onModifiersChanged(
      sigc::hide(sigc::mem_fun(this, &PanelUnitPresetMode::bruteForceUpdateLeds)));

  Application::get().getPresetManager()->getEditBuffer()->onChange(
      mem_fun(this, &PanelUnitPresetMode::bruteForceUpdateLeds));
}

PanelUnitPresetMode::~PanelUnitPresetMode()
{
  Application::get().getSettings()->getSetting<ForceHighlightChangedParametersSetting>()->set(
      BooleanSetting::tEnum::BOOLEAN_SETTING_FALSE);
  DebugLevel::gassy(__PRETTY_FUNCTION__);
}

void PanelUnitPresetMode::bruteForceUpdateLeds()
{
  m_bruteForceLedThrottler.doTask([this]() {
    if(Application::get().getHWUI()->getPanelUnit().getUsageMode().get() != this)
      return;

    std::array<TwoStateLED::LedState, numLeds> states{ TwoStateLED::OFF };

    if(Application::get().getHWUI()->getButtonModifiers()[SHIFT] == true)
      getMappings().forEachButton([&](Buttons buttonId, const std::list<int>& parameters) {
        Application::get().getSettings()->getSetting<ForceHighlightChangedParametersSetting>()->set(
            BooleanSetting::tEnum::BOOLEAN_SETTING_TRUE);
        letChangedButtonsBlink(buttonId, parameters, states);
      });
    else
      getMappings().forEachButton([&](Buttons buttonId, const std::list<int>& parameters) {
        Application::get().getSettings()->getSetting<ForceHighlightChangedParametersSetting>()->set(
            BooleanSetting::tEnum::BOOLEAN_SETTING_FALSE);
        setStateForButton(buttonId, parameters, states);
      });

    applyStateToLeds(states);
  });
}

void PanelUnitPresetMode::letChangedButtonsBlink(Buttons buttonId, const std::list<int>& parameters,
                                                 std::array<TwoStateLED::LedState, numLeds>& states)
{
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto currentParams = getMappings().findParameters(buttonId);
  auto ebParameters = editBuffer->getParametersSortedById();

  bool anyChanged = false;
  for(const auto paramID : currentParams)
  {
    anyChanged |= ebParameters[paramID]->isChangedFromLoaded();
  }
  states[(int) buttonId] = anyChanged ? TwoStateLED::BLINK : TwoStateLED::OFF;
}

void PanelUnitPresetMode::setStateForButton(Buttons buttonId, const std::list<int>& parameters,
                                            std::array<TwoStateLED::LedState, numLeds>& states)
{
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

  for(const auto i : parameters)
  {
    auto signalFlowIndicator = ParameterDB::get().getSignalPathIndication(i);
    auto parameter = editBuffer->findParameterByID(static_cast<size_t>(i));

    if(auto mc = dynamic_cast<MacroControlParameter*>(parameter))
    {
      if(!mc->getTargets().empty())
      {
        states[(int) buttonId] = TwoStateLED::ON;
        break;
      }
    }
    else if(signalFlowIndicator != invalidSignalFlowIndicator)
    {
      if(parameter->getControlPositionValue() != signalFlowIndicator)
      {
        states[(int) buttonId] = TwoStateLED::ON;
        break;
      }
    }
  }
}

void PanelUnitPresetMode::applyStateToLeds(std::array<TwoStateLED::LedState, numLeds>& states)
{
  auto& panelUnit = Application::get().getHWUI()->getPanelUnit();
  for(int i = 0; i < numLeds; i++)
  {
    panelUnit.getLED((Buttons) i)->setState(states[i]);
  }
}
