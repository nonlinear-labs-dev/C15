#include "BasicItem.h"

bool BasicItem::canEnter()
{
  return false;
}

bool BasicItem::redraw(FrameBuffer& fb)
{
  auto ret = ControlWithChildren::redraw(fb);
  ret |= drawHighlightBorder(fb);
  return ret;
}

bool BasicItem::drawHighlightBorder(FrameBuffer& fb)
{
  if(!isHighlight())
    return false;

  fb.setColor(FrameBuffer::C103);
  fb.drawRect(getPosition());
  return true;
}
