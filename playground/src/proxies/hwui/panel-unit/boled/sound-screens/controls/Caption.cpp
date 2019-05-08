#include <proxies/hwui/panel-unit/boled/sound-screens/controls/Caption.h>
#include "proxies/hwui/panel-unit/boled/BOLED.h"

Caption::Caption(const Glib::ustring &text, const Rect &pos)
    : super(text, pos)
{
}

Caption::~Caption()
{
}

bool Caption::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C128);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}

void Caption::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

shared_ptr<Font> Caption::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int Caption::getFontHeight() const
{
  return 8;
}
