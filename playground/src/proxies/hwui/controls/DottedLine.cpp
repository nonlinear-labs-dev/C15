#include "DottedLine.h"

DottedLine::DottedLine(const Rect &rect)
    : super(rect)
{
}

DottedLine::~DottedLine()
{
}

bool DottedLine::redraw(FrameBuffer &fb)
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C128);
  else
    fb.setColor(FrameBuffer::Colors::C204);

  Rect r = getPosition();
  Point c = r.getCenter();

  for(int i = r.getLeft(); i <= r.getRight(); i += 2)
    fb.setPixel(i, r.getY());

  return true;
}
