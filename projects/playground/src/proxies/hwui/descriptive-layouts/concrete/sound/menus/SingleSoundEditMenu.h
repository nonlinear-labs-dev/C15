#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/SoundEditMenu.h>

class SingleSoundEditMenu : public SoundEditMenu
{
 public:
  explicit SingleSoundEditMenu(const Rect& r);
  void init() override;

 protected:
  int getDefaultItemHeight() const override;
};
