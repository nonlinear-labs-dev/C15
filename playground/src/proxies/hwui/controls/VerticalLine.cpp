#include "VerticalLine.h"

VerticalLine::VerticalLine(const Rect &rect)
    : super(rect)
{
}

VerticalLine::~VerticalLine()
{
}

bool VerticalLine::redraw(FrameBuffer &fb)
{
  fb.setColor(FrameBuffer::Colors::C128);

  Rect r = getPosition();
  Point c = r.getCenter();

  fb.drawVerticalLine(c.getX(), r.getY(), r.getHeight());
  return true;
}
