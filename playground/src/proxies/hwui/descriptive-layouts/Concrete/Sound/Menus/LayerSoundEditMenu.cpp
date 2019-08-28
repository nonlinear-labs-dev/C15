#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/ConvertToSingleItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/InitSound.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/RandomizeItem.h>
#include "LayerSoundEditMenu.h"

LayerSoundEditMenu::LayerSoundEditMenu(const Rect &r) : ScrollMenu(r) {
  init();
}

void LayerSoundEditMenu::init() {
  addItem<ConvertToSingleItem>();
  addItem<InitSound>();
  addItem<RandomizeItem>();
}
