#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/Bank.h>
#include <presets/EditBuffer.h>
#include <device-settings/BaseUnitUIMode.h>
#include <device-settings/Settings.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <playground.h>
#include <proxies/hwui/base-unit/BaseUnitPresetsAndBanksMode.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUIEnums.h>

BaseUnitPresetsAndBanksMode::BaseUnitPresetsAndBanksMode()
    : m_modeButtonHandler(std::bind(&BaseUnitPresetsAndBanksMode::modeButtonShortPress, this),
                          std::bind(&BaseUnitPresetsAndBanksMode::modeButtonLongPress, this))
    , m_funcButtonHandler([] {},
                          [] { Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->toggle(); })
{
}

void BaseUnitPresetsAndBanksMode::setup()
{
  setupButtonConnection(Buttons::BUTTON_FUNCTION, [=](auto, auto, auto state) {
    if(state)
      onFuncButtonDown();

    m_funcButtonHandler.onButtonEvent(state);
    return true;
  });

  setupButtonConnection(Buttons::BUTTON_MODE, [=](auto, auto, auto state) {
    m_modeButtonHandler.onButtonEvent(state);
    return true;
  });
}

void BaseUnitPresetsAndBanksMode::modeButtonShortPress()
{
  auto s = Application::get().getSettings()->getSetting<BaseUnitUIMode>();

  if(s->get() == BaseUnitUIModes::Presets)
    s->set(BaseUnitUIModes::Banks);
  else
    s->set(BaseUnitUIModes::Presets);
}

void BaseUnitPresetsAndBanksMode::modeButtonLongPress()
{
  Application::get().getSettings()->getSetting<BaseUnitUIMode>()->set(BaseUnitUIModes::Play);
}

void BaseUnitPresetsAndBanksMode::onLongPress()
{
}

void BaseUnitPresetsAndBanksMode::onFuncButtonDown()
{
}
