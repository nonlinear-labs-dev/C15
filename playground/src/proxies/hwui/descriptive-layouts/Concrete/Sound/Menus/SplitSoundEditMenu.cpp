#include "SplitSoundEditMenu.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <device-settings/RandomizeAmount.h>
#include <proxies/hwui/controls/Button.h>

#include <device-settings/TransitionTime.h>
#include <device-settings/EditSmoothingTime.h>

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/MenuItems/GenericItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/MenuItems/SettingItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/RandomizeItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/ConvertToSingleItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/InitSound.h>

SplitSoundEditMenu::SplitSoundEditMenu(const Rect& r)
    : ScrollMenu(r)
{
  init();
}

void SplitSoundEditMenu::init()
{
  addItem<GenericItem>("Import Preset", []() {
    Application::get().getHWUI()->setFocusAndMode(UIDetail::SoundSelectPresetForVoiceGroup);
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().bruteForce();
  });

  addItem<ConvertToSingleItem>();
  addItem<InitSound>();
  addItem<RandomizeItem>();

  addItem<SettingItem<TransitionTime>>("Transition Time");
  addItem<SettingItem<EditSmoothingTime>>("Smoothing Time");
}
