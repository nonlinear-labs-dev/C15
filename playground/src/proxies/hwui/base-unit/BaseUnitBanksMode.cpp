#include <Application.h>
#include <presets/PresetManager.h>
#include <device-settings/BaseUnitUIMode.h>
#include <device-settings/Settings.h>
#include <playground.h>
#include <proxies/hwui/base-unit/BaseUnitBanksMode.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUIEnums.h>

BaseUnitBanksMode::BaseUnitBanksMode ()
{
}

BaseUnitBanksMode::~BaseUnitBanksMode ()
{
}

void BaseUnitBanksMode::setup ()
{
  super::setup();

  setupButtonConnection (Buttons::BUTTON_MINUS, [=](Buttons button, ButtonModifiers modifiers, bool state)
  {
    if(state)
    {
      Application::get().getPresetManager()->undoableSelectPrevious ();
    }
    return true;
  });

  setupButtonConnection (Buttons::BUTTON_PLUS, [=](Buttons button, ButtonModifiers modifiers, bool state)
  {
    if(state)
    {
      Application::get().getPresetManager()->undoableSelectNext ();
    }
    return true;
  });
}

