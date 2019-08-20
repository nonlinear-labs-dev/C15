#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/ScrollMenu.h>

class SplitSoundEditMenu : public ScrollMenu {
public:
  SplitSoundEditMenu(const Rect& r);
  void init() override;

};
