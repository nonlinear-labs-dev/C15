#include "ButtonMenuButton.h"
#include <proxies/hwui/FrameBuffer.h>

ButtonMenuButton::ButtonMenuButton(bool isFirst, bool isLast, const Glib::ustring &text, const Rect &pos)
    : super(StringAndSuffix { text }, pos)
    , m_isFirst(isFirst)
    , m_isLast(isLast)
    , m_justification(Font::Justification::Center)
{
}

ButtonMenuButton::~ButtonMenuButton() = default;

void ButtonMenuButton::drawBackground(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  if(isHighlight())
  {
    fb.setColor(FrameBufferColors::C128);
    fb.fillRect(r.getX() + 1, r.getTop() + 1, r.getWidth() - 2, r.getHeight() - 2);
  }
  else
  {
    super::drawBackground(fb);
  }
}

Font::Justification ButtonMenuButton::getJustification() const
{
  return m_justification;
}

void ButtonMenuButton::setJustification(Font::Justification justi)
{
  m_justification = justi;
}
