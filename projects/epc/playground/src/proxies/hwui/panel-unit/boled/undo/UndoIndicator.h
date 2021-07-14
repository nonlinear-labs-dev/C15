#pragma once

#include <proxies/hwui/controls/Label.h>

class UndoIndicator : public Label
{
 public:
  explicit UndoIndicator(const Rect &pos);
  bool redraw(FrameBuffer &fb) override;
};
