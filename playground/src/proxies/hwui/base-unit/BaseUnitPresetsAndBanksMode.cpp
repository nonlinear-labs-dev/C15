#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/PresetBank.h>
#include <presets/EditBuffer.h>
#include <device-settings/BaseUnitUIMode.h>
#include <device-settings/Settings.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <playground.h>
#include <proxies/hwui/base-unit/BaseUnitPresetsAndBanksMode.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUIEnums.h>

BaseUnitPresetsAndBanksMode::BaseUnitPresetsAndBanksMode () :
    m_fireLongPress (bind (&BaseUnitPresetsAndBanksMode::onLongPress, this))
{
}

BaseUnitPresetsAndBanksMode::~BaseUnitPresetsAndBanksMode ()
{
}

void BaseUnitPresetsAndBanksMode::setup ()
{
  setupButtonConnection (Buttons::BUTTON_FUNCTION, [=](Buttons button, ButtonModifiers modifiers, bool state)
  {
    if(state)
    {
      onFuncButtonDown();
      m_fireLongPress.refresh(std::chrono::milliseconds(600));
    }
    else
    {
      m_fireLongPress.cancel();
    }

    return true;
  });

  setupButtonConnection (Buttons::BUTTON_MODE, [ = ] (Buttons buttonID, ButtonModifiers modifiers, bool state)
  {
    if (state)
    {
      Application::get ().getSettings ()->getSetting<BaseUnitUIMode> ()->inc ();
    }

    return true;
  });
}

void BaseUnitPresetsAndBanksMode::onLongPress ()
{
  Application::get ().getSettings ()->getSetting<AutoLoadSelectedPreset> ()->toggle ();
}

void BaseUnitPresetsAndBanksMode::onFuncButtonDown ()
{
}

