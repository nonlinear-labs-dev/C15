#pragma once

#include "SoundEditMenu.h"

class SplitSoundEditMenu : public SoundEditMenu {
public:
  explicit SplitSoundEditMenu(const Rect& r);
  void init() override;
};
