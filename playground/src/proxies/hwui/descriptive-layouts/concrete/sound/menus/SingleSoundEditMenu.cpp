#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/GenericItem.h>
#include <proxies/hwui/descriptive-layouts/concrete/sound/menus/items/InitSound.h>
#include <proxies/hwui/descriptive-layouts/concrete/sound/menus/items/RandomizeItem.h>
#include <proxies/hwui/descriptive-layouts/ConditionRegistry.h>
#include "SingleSoundEditMenu.h"
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/TextItem.h>

SingleSoundEditMenu::SingleSoundEditMenu(const Rect& r)
    : ScrollMenu(r)
{
  init();
}

void SingleSoundEditMenu::init()
{
  auto fullWidth = Rect{0, 0, 254, 96};
  addItem<TextItem>("-- Single --", fullWidth);
  addItem<InitSound>(fullWidth);
  addItem<RandomizeItem>(fullWidth);
}
