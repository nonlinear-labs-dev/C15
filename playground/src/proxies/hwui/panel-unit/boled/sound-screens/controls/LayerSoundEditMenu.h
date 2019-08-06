#pragma once

#include "SingleSoundEditMenu.h"

class LayerSoundEditMenu : public SingleSoundEditMenu
{
 public:
  explicit LayerSoundEditMenu(const Rect& rect);
  void init() override;
};