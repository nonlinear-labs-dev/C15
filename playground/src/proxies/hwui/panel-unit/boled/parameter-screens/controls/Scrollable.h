#pragma once

#include <playground.h>
#include <tools/Signal.h>

class Rect;
class ScrollArea;

class Scrollable
{
 public:
  virtual void setPosition(const Rect &rect) = 0;
  virtual const Rect &getPosition() const = 0;
  virtual void setDirty() = 0;

  void setArea(ScrollArea *area);

  connection onDirtyStateChanged(slot<void, bool> cb);

 protected:
  void notifyDirty(bool b);
  void scrollTop();
  ScrollArea *m_area;

 private:
  Signal<void, bool> m_sigDirty;
};
