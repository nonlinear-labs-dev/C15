#include "InvertedLabel.h"

InvertedLabel::InvertedLabel(const Glib::ustring& text, const Rect& pos)
    : super(text, pos)
{
  setFontColor(FrameBuffer::Colors::C43);
}

InvertedLabel::InvertedLabel(const Rect& pos)
    : super(pos)
{
}

InvertedLabel::~InvertedLabel()
{
}

bool InvertedLabel::redraw(FrameBuffer& fb)
{
  auto r = getPosition();
  fb.setColor(FrameBuffer::Colors::C255);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());
  super::redraw(fb);
  return true;
}
