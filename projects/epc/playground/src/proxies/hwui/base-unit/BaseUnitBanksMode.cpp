#include <Application.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/base-unit/BaseUnitBanksMode.h>
#include <proxies/hwui/buttons.h>
#include <device-settings/Settings.h>
#include <device-settings/DirectLoadSetting.h>

void BaseUnitBanksMode::setup()
{
  super::setup();

  setupButtonConnection(Buttons::BUTTON_MINUS, [=](auto, auto, auto state) {
    if(state)
      installButtonRepeat([] {
        PresetManagerUseCases useCase(Application::get().getPresetManager());
        useCase.selectPreviousBank();
      });
    else
      removeButtonRepeat();

    return true;
  });

  setupButtonConnection(Buttons::BUTTON_PLUS, [=](auto, auto, auto state) {
    if(state)
      installButtonRepeat([] {
        PresetManagerUseCases useCase(Application::get().getPresetManager());
        useCase.selectNextBank();
      });
    else
      removeButtonRepeat();

    return true;
  });
}
