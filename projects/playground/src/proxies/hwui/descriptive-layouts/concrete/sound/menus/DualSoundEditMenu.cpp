#include "DualSoundEditMenu.h"

#include <proxies/hwui/descriptive-layouts/concrete/sound/menus/items/ConvertToSoundTypeItem.h>
#include <proxies/hwui/descriptive-layouts/concrete/sound/menus/items/PartLabelItem.h>
#include <proxies/hwui/descriptive-layouts/concrete/sound/menus/items/InitSound.h>

#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>

DualSoundEditMenu::DualSoundEditMenu(const Rect &r)
    : ScrollMenu(r)
{
  init();
}

void DualSoundEditMenu::init()
{
  auto fullWidth = Rect { 0, 0, 256, 12 };
  addItem<PartLabelItem>(fullWidth);
  addItem<ConvertToSoundTypeItem>(fullWidth, SoundType::Single);
  addItem<InitPart>(fullWidth);
}

int DualSoundEditMenu::getDefaultItemHeight() const
{
  return 12;
}
