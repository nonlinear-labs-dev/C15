#include "EmptyBankLabel.h"
#include <proxies/hwui/FrameBuffer.h>

EmptyBankLabel::EmptyBankLabel(const Rect &pos)
    : super("empty", pos)
{
}

EmptyBankLabel::~EmptyBankLabel()
{
}

void EmptyBankLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C179);
}

bool EmptyBankLabel::redraw(FrameBuffer &fb)
{
  super::redraw(fb);
  fb.setColor(FrameBufferColors::C179);
  const Rect &r = getPosition();
  fb.drawRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
  return true;
}
