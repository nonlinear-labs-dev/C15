#include "Overlay.h"

Overlay::Overlay(const Rect &pos)
    : super(pos)
{
}

void Overlay::drawBackground(FrameBuffer &fb)
{
}

void Overlay::setBackgroundColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

bool Overlay::isTransparent() const
{
  return true;
}
