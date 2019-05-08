#pragma once

#include "Control.h"

class VerticalDottedLine : public Control
{
 private:
  typedef Control super;

 public:
  VerticalDottedLine(const Rect& rect);
  virtual ~VerticalDottedLine();

  bool redraw(FrameBuffer& fb);

 private:
  VerticalDottedLine(const VerticalDottedLine& other);
  VerticalDottedLine& operator=(const VerticalDottedLine&);
};
