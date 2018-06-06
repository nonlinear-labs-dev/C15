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

PanelUnitPresetMode::PanelUnitPresetMode() :
    m_bruteForceLedThrottler(std::chrono::milliseconds(40))
{
  DebugLevel::gassy (__PRETTY_FUNCTION__);

  Application::get().getPresetManager()->getEditBuffer()->onChange(mem_fun(this, &PanelUnitPresetMode::bruteForceUpdateLeds));

}

PanelUnitPresetMode::~PanelUnitPresetMode ()
{
  DebugLevel::gassy (__PRETTY_FUNCTION__);
}

void PanelUnitPresetMode::bruteForceUpdateLeds ()
{
  m_bruteForceLedThrottler.doTask([this]()
  {
    array<TwoStateLED::LedState, numLeds> states;
    states.fill(TwoStateLED::OFF);

    getMappings().forEachButton ([&](Buttons buttonId, const list<int> parameters)
    {
      setStateForButton(buttonId, parameters, states);
    });

    applyStateToLeds(states);
  });
}

void PanelUnitPresetMode::setStateForButton(Buttons buttonId, const list<int> parameters, array<TwoStateLED::LedState, numLeds>& states)
{
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

  for (auto i : parameters)
  {
    auto signalFlowIndicator = ParameterDB::get().getSignalPathIndication(i);

    if (auto mc = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(i)))
    {
      if (mc->getTargets().size() > 0)
      {
        states[(int)buttonId] = TwoStateLED::ON;
      }
    }
    else if (signalFlowIndicator != invalidSignalFlowIndicator)
    {
      if (auto p = editBuffer->findParameterByID(i))
      {
        if (p->getControlPositionValue() != signalFlowIndicator)
        {
          states[(int)buttonId] = TwoStateLED::ON;
        }
      }
    }
  }
}

void PanelUnitPresetMode::applyStateToLeds(array<TwoStateLED::LedState, numLeds>& states)
{
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();
  for (int i = 0; i < numLeds; i++)
  {
    panelUnit.getLED((Buttons)i)->setState(states[i]);
  }
}
