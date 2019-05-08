#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class SetupSelectionEntry : public ControlWithChildren
{
  typedef ControlWithChildren super;

 public:
  SetupSelectionEntry(int i, const Rect &pos);
  virtual ~SetupSelectionEntry();

  void assign(Control *s, bool selectMode);
  bool redraw(FrameBuffer &fb) override;
  void setBackgroundColor(FrameBuffer &fb) const override;

 private:
  bool m_selectMode = false;
};
