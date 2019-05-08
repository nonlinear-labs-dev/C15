#pragma once

#include "proxies/hwui/controls/Button.h"

class Application;

class PCAmountButton : public Button
{
 private:
  typedef Button super;

 public:
  // public methods
  PCAmountButton(const Rect& rect);
  virtual ~PCAmountButton();

 private:
  // private methods
  PCAmountButton(const PCAmountButton& other);
  PCAmountButton& operator=(const PCAmountButton&);
};
