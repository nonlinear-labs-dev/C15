#pragma once

#include "Control.h"

class VerticalLine : public Control
{
 private:
  typedef Control super;

 public:
  VerticalLine(const Rect& rect);
  virtual ~VerticalLine();

  bool redraw(FrameBuffer& fb);

 private:
  VerticalLine(const VerticalLine& other);
  VerticalLine& operator=(const VerticalLine&);
};
