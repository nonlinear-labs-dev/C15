#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/ScrollMenu.h>

class SingleSoundEditMenu : public ScrollMenu
{
 public:
  explicit SingleSoundEditMenu(const Rect& r);
  void init() override;

 protected:
  int getDefaultItemHeight() const override;
};
