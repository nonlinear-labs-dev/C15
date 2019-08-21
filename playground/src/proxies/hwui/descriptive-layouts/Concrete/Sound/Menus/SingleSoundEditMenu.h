#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/ScrollMenu.h>

class SingleSoundEditMenu : public ScrollMenu {
public:
  explicit SingleSoundEditMenu(const Rect& r);
  void init() override;
};
