#pragma once

#include "Control.h"

class DottedLine : public Control
{
 private:
  typedef Control super;

 public:
  explicit DottedLine(const Rect& rect);
  bool redraw(FrameBuffer& fb) override;

 private:
  DottedLine(const DottedLine& other);
  DottedLine& operator=(const DottedLine&);
};
