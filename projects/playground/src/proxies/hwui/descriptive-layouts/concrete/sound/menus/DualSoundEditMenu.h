#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/ScrollMenu.h>

class DualSoundEditMenu : public ScrollMenu
{
 public:
  explicit DualSoundEditMenu(const Rect& r);
  void init() override;

 protected:
  int getDefaultItemHeight() const override;
};
