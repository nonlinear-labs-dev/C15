#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/ScrollMenu.h>

class SoundEditMenu : public ScrollMenu {
public:
  SoundEditMenu(const Rect& r);
  void onActionPerformed() override;
};
