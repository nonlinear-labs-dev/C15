#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class Scrollable;

class ScrollArea : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  ScrollArea(Scrollable *content, const Rect &position);
  void scroll(int diff);

 private:
  Scrollable *m_content = nullptr;
};
