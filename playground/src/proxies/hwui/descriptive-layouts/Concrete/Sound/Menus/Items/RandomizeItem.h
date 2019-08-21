#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/MenuItems/EditorItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/Overlays/RandomizeOverlay.h>

class RandomizeItem : public EditorItem
{
public:
  RandomizeItem();
  ~RandomizeItem() override;
  ScrollMenuOverlay* createOverlay() override;
};