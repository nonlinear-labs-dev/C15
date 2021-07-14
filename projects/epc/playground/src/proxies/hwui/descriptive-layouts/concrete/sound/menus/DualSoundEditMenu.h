#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/ScrollMenu.h>

class DualSoundEditMenu : public ScrollMenu
{
 public:
  explicit DualSoundEditMenu(const Rect& r);
  void init() override;

 protected:
  void initSplit();
  void initLayer();
  int getDefaultItemHeight() const override;
};
