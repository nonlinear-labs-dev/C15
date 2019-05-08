#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"
#include "device-settings/PresetStoreModeSetting.h"
#include <libundo/undo/Scope.h>

class SingleSoundMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  SingleSoundMenu(const Rect &rect);
  virtual ~SingleSoundMenu();
};
