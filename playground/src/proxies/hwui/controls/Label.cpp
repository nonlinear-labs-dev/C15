#include "Label.h"
#include "proxies/hwui/Oleds.h"

Label::Label (const Glib::ustring &text, const Rect &pos) :
    super (pos),
    m_text(text)
{
}

Label::Label (const Rect &pos) :
    Label ("", pos)
{
}

Label::~Label ()
{
}

void Label::setFontColor (FrameBuffer::Colors color)
{
  m_fontColor = color;
}

void Label::setFontColor (FrameBuffer &fb) const
{
  if (m_fontColor != FrameBuffer::Colors::Undefined)
  {
    fb.setColor (m_fontColor);
  }
  else if (isHighlight ())
  {
    fb.setColor (FrameBuffer::Colors::C255);
  }
  else
  {
    fb.setColor (FrameBuffer::Colors::C128);
  }
}

void Label::setSuffixFontColor (FrameBuffer &fb) const
{
  setFontColor (fb);
}

bool Label::setText (const Glib::ustring &text, int suffixLength)
{
  if (m_text != text || m_suffixLength != suffixLength)
  {
    m_text = text;
    m_suffixLength = suffixLength;
    setDirty ();
    return true;
  }
  return false;
}

Glib::ustring Label::getText () const
{
  return m_text;
}

Glib::ustring Label::shortenStringIfNeccessary (shared_ptr<Font> font, const Glib::ustring &text) const
{
  return text;
}

bool Label::redraw (FrameBuffer &fb)
{
  auto pos = getPosition ();
  auto font = getFont ();
  auto clip = fb.clipRespectingOffset (pos);

  if(clip.isEmpty())
    return false;

  setFontColor (fb);

  int offset = (pos.getHeight () - getFontHeight ()) / 2;

  Glib::ustring text = getText ();

  int width = font->getStringWidth (text);

  if (width >= (pos.getWidth () - getXOffset() - getRightMargin()))
  {
    text = shortenStringIfNeccessary (font, text);
    width = font->getStringWidth (text);
  }

  Glib::ustring firstPart = text;
  Glib::ustring secondPart = "";

  if (m_suffixLength)
  {
    firstPart = text.substr (0, text.length () - m_suffixLength);
    secondPart = text.substr (text.length () - m_suffixLength);
  }

  auto left = pos.getX () + getXOffset ();

  switch (getJustification ())
  {
    case Font::Justification::Center:
      font->draw (firstPart, left + (pos.getWidth() - width) / 2, pos.getBottom () - offset + getYOffset ());
      break;

    case Font::Justification::Left:
      font->draw (firstPart, left, pos.getBottom () - offset + getYOffset ());
      break;

    case Font::Justification::Right:
      font->draw (firstPart, left + (pos.getWidth() - width - getRightMargin()), pos.getBottom () - offset + getYOffset ());
      break;
  }

  if (!secondPart.empty ())
  {
    drawSuffix (fb, width, firstPart, secondPart);
  }

  super::redraw (fb);
  return true;
}

void Label::drawSuffix (FrameBuffer &fb, int fullWidth, const Glib::ustring &firstPart, const Glib::ustring &secondPart)
{
  auto pos = getPosition ();
  auto font = getFont ();
  int offset = (pos.getHeight () - getFontHeight ()) / 2;

  int firstPartWidth = font->getStringWidth (firstPart);

  setSuffixFontColor (fb);

  auto left = pos.getX () + getXOffset () + firstPartWidth;

  switch (getJustification ())
  {
    case Font::Justification::Center:
      font->draw (secondPart, left + (pos.getWidth () - fullWidth) / 2, pos.getBottom () - offset + getYOffset ());
      break;

    case Font::Justification::Left:
      font->draw (secondPart, left, pos.getBottom () - offset + getYOffset ());
      break;

    case Font::Justification::Right:
      font->draw (secondPart, left + (pos.getWidth () - fullWidth), pos.getBottom () - offset + getYOffset ());
      break;
  }
}

shared_ptr<Font> Label::getFont () const
{
  return Oleds::get ().getFont ("Emphase_9_Regular", getFontHeight ());
}

int Label::getFontHeight () const
{
  return 9;
}

int Label::getXOffset () const
{
  return 0;
}

int Label::getYOffset () const
{
  return 0;
}

int Label::getRightMargin() const
{
  return 0;
}

Font::Justification Label::getJustification () const
{
  return Font::Justification::Center;
}
