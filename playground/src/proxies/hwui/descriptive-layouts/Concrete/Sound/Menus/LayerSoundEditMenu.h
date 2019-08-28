#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/ScrollMenu.h>

class LayerSoundEditMenu : public ScrollMenu {
public:
  explicit LayerSoundEditMenu(const Rect& r);
  void init() override;
};

