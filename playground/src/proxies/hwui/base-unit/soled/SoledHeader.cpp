#include <proxies/hwui/base-unit/soled/SoledHeader.h>

SoledHeader::SoledHeader(const Glib::ustring &text, const Rect &pos)
    : base_type(text, pos)
{
}

SoledHeader::~SoledHeader()
{
}

void SoledHeader::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::C43);
}

bool SoledHeader::redraw(FrameBuffer &fb)
{
  auto pos = getPosition();
  fb.setColor(FrameBuffer::C255);
  fb.fillRect(pos);
  Label::redraw(fb);
  return true;
}

int SoledHeader::getXOffset() const
{
  return 1;
}
