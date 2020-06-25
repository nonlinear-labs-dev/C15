#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/SoundEditMenu.h>

class DualSoundEditMenu : public SoundEditMenu
{
 public:
  explicit DualSoundEditMenu(const Rect& r);
  void init() override;

 protected:

  void initSplit();
  void initLayer();
  int getDefaultItemHeight() const override;
};
