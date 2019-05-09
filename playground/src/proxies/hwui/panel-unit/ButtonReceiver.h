#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/buttons.h>

class ButtonReceiver
{
 public:
  virtual ~ButtonReceiver() = default;
  virtual bool onButton(Buttons i, bool down, ButtonModifiers mod) = 0;
};
