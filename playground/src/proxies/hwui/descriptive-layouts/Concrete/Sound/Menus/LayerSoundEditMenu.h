#pragma once
#include "SoundEditMenu.h"


class LayerSoundEditMenu : public SoundEditMenu {
public:
  explicit LayerSoundEditMenu(const Rect& r);
  void init() override;
};

