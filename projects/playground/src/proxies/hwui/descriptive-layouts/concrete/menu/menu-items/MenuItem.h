#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class MenuItem : public ControlWithChildren
{
 public:
  explicit MenuItem(const Rect& rect);

  virtual bool canEnter() = 0;
  virtual void doAction() = 0;
  virtual Control* createInfo() = 0;
};
