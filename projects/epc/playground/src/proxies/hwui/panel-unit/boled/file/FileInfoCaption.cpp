#include "FileInfoCaption.h"
#include <proxies/hwui/FrameBuffer.h>

FileInfoCaption::FileInfoCaption(const Glib::ustring &str, const Rect &pos)
    : super(pos)
{
  super::setText(StringAndSuffix { str });
  super::setFontColor(FrameBufferColors::C43);
}

bool FileInfoCaption::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBufferColors::C128);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}
