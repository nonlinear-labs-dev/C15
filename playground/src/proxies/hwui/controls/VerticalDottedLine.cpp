#include "VerticalDottedLine.h"

VerticalDottedLine::VerticalDottedLine(const Rect &rect)
    : super(rect)
{
}

VerticalDottedLine::~VerticalDottedLine()
{
}

bool VerticalDottedLine::redraw(FrameBuffer &fb)
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C128);
  else
    fb.setColor(FrameBuffer::Colors::C204);

  Rect r = getPosition();
  Point c = r.getCenter();

  for(int i = r.getTop(); i <= r.getBottom(); i += 2)
    fb.setPixel(r.getX(), i);

  return true;
}
