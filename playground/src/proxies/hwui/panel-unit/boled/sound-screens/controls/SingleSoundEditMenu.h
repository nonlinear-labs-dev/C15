#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"
#include "device-settings/PresetStoreModeSetting.h"
#include <libundo/undo/Scope.h>

class SingleSoundEditMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  SingleSoundEditMenu(const Rect &rect);
  virtual ~SingleSoundEditMenu();
};
