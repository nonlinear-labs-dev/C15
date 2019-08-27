#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/ConvertToSingleItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/InitSound.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/RandomizeItem.h>

#include <proxies/hwui/HWUI.h>

#include "LayerSoundEditMenu.h"

LayerSoundEditMenu::LayerSoundEditMenu(const Rect &r) : SoundEditMenu(r) {
  init();
}

void LayerSoundEditMenu::init() {
  addItem<ConvertToSingleItem>();
  addItem<InitSound>();
  addItem<RandomizeItem>();
}
