#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/Bank.h>
#include <presets/EditBuffer.h>
#include <device-settings/Settings.h>
#include <device-settings/DirectLoadSetting.h>
#include <playground.h>
#include <proxies/hwui/base-unit/BaseUnitPresetsMode.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUI.h>

void BaseUnitPresetsMode::setup()
{
  super::setup();

  setupButtonConnection(Buttons::BUTTON_MINUS, [=](auto, auto, auto state) {
    if(state)
    {
      installButtonRepeat([] {
        if(auto bank = Application::get().getPresetManager()->getSelectedBank())
          bank->selectPreviousPreset();
      });
    }
    else
    {
      removeButtonRepeat();
    }

    return true;
  });

  setupButtonConnection(Buttons::BUTTON_PLUS, [=](auto, auto, auto state) {
    if(state)
    {
      installButtonRepeat([] {
        if(auto bank = Application::get().getPresetManager()->getSelectedBank())
          bank->selectNextPreset();
      });
    }
    else
    {
      removeButtonRepeat();
    }

    return true;
  });
}

void BaseUnitPresetsMode::onFuncButtonDown()
{
  auto &app = Application::get();
  auto eb = app.getPresetManager()->getEditBuffer();
  auto currentVoiceGroup = Application::get().getHWUI()->getCurrentVoiceGroup();
  eb->undoableLoadSelectedPreset(currentVoiceGroup);
}
