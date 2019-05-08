#include "InfoLabel.h"
#include <proxies/hwui/Oleds.h>

InfoLabel::InfoLabel(const Rect &pos)
    : super("Info", pos)
{
}

shared_ptr<Font> InfoLabel::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int InfoLabel::getFontHeight() const
{
  return 8;
}

void InfoLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C179);
}

bool InfoLabel::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C77);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}
