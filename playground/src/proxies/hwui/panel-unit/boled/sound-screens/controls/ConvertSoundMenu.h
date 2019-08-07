#pragma once

#include <presets/EditBuffer.h>
#include "proxies/hwui/controls/ButtonMenu.h"

class ConvertSoundMenu : public ButtonMenu
{
 public:
  explicit ConvertSoundMenu(const Rect& rect);

 protected:
  void setup();
  void convertSoundTo(Type newType);
};
