#include <Application.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/base-unit/BaseUnitBanksMode.h>
#include <proxies/hwui/buttons.h>

void BaseUnitBanksMode::setup()
{
  super::setup();

  setupButtonConnection(Buttons::BUTTON_MINUS, [=](auto, auto, auto state) {
    if(state)
      Application::get().getPresetManager()->selectPreviousBank();

    return true;
  });

  setupButtonConnection(Buttons::BUTTON_PLUS, [=](auto, auto, auto state) {
    if(state)
      Application::get().getPresetManager()->selectNextBank();

    return true;
  });
}
