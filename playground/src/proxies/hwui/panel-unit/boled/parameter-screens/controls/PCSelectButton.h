#pragma once

#include "proxies/hwui/controls/Button.h"

class Application;

class PCSelectButton : public Button
{
 private:
  typedef Button super;

 public:
  // public methods
  PCSelectButton(const Rect& rect);
  virtual ~PCSelectButton();

 private:
  // private methods
  PCSelectButton(const PCSelectButton& other);
  PCSelectButton& operator=(const PCSelectButton&);
};
