#pragma once
#include "SoundEditMenu.h"

class SingleSoundEditMenu : public SoundEditMenu {
public:
  explicit SingleSoundEditMenu(const Rect& r);
  void init() override;
};
