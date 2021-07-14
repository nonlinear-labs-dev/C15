#include "InfoLabel.h"
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/FrameBuffer.h>

InfoLabel::InfoLabel(const Rect &pos, const Glib::ustring &text)
    : super(StringAndSuffix { "Info " + text }, pos)
{
}

std::shared_ptr<Font> InfoLabel::getFont() const
{
  return Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
}

int InfoLabel::getFontHeight() const
{
  return 8;
}

void InfoLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C179);
}

bool InfoLabel::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBufferColors::C77);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}
