#pragma once

#include "proxies/hwui/controls/Button.h"

class Application;

class PCPositionButton : public Button
{
 private:
  typedef Button super;

 public:
  // public methods
  PCPositionButton(const Rect& rect);
  virtual ~PCPositionButton();

 private:
  // private methods
  PCPositionButton(const PCPositionButton& other);
  PCPositionButton& operator=(const PCPositionButton&);
};
