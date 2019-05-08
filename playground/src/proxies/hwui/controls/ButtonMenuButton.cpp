#include "ButtonMenuButton.h"

ButtonMenuButton::ButtonMenuButton(bool isFirst, bool isLast, const Glib::ustring &text, const Rect &pos)
    : super(text, pos)
    , m_isFirst(isFirst)
    , m_isLast(isLast)
{
}

ButtonMenuButton::~ButtonMenuButton()
{
}

void ButtonMenuButton::drawBackground(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  if(isHighlight())
  {
    fb.setColor(FrameBuffer::Colors::C128);
    fb.fillRect(r.getX() + 1, r.getTop() + 1, r.getWidth() - 2, r.getHeight() - 2);
  }
  else
  {
    super::drawBackground(fb);
  }
}
