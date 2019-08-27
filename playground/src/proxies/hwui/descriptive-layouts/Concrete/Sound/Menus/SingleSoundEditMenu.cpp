#include <proxies/hwui/descriptive-layouts/Concrete/Menu/MenuItems/GenericItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/InitSound.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Items/RandomizeItem.h>
#include "SingleSoundEditMenu.h"

SingleSoundEditMenu::SingleSoundEditMenu(const Rect& r) : SoundEditMenu(r)
{
  init();
}

void SingleSoundEditMenu::init() {
  addItem<InitSound>();
  addItem<RandomizeItem>();
}
