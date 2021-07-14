#pragma once

#include <proxies/hwui/controls/Control.h>

class Scrollable;

class Scrollbar : public Control
{
 private:
  typedef Control super;

 public:
  Scrollbar(Scrollable *content, const Rect &pos);

  bool redraw(FrameBuffer &fb) override;

 private:
  void onClientsDirtyStateChanged(bool isDirty);

  Scrollable *m_content = nullptr;
};
