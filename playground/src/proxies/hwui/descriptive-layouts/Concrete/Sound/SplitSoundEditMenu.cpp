#include "SplitSoundEditMenu.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/AbstractMenuItems/EditorItem.h>
#include <device-settings/RandomizeAmount.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/controls/ArrowLeft.h>
#include <proxies/hwui/controls/ArrowRight.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/SoundOverlays/ArrowIncrementDecrementOverlay.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/MenuItems/RandomizeItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/AbstractMenuItems/GenericItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/MenuItems/ConvertToSingleItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/MenuItems/InitSoundItem.h>

SplitSoundEditMenu::SplitSoundEditMenu(const Rect& r)
    : ScrollMenu(r)
{
  init();
}

void SplitSoundEditMenu::init()
{
  addItem<GenericItem>("Import Preset", []() {
    Application::get().getHWUI()->setFocusAndMode(UIDetail::SoundSelectPresetForVoiceGroup);
  });
  addItem<ConvertToSingle>();
  addItem<InitSound>();
  addItem<RandomizeEditor>();
  // addItem<MonoModeEditor>();
  //addItem<TransitionTimeEditor>();
  //addItem<TuneReferenceEditor>();
}
