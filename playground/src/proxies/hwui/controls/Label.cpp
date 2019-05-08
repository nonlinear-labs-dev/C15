#include "Label.h"
#include "proxies/hwui/Oleds.h"

Label::Label(const StringAndSuffix &text, const Rect &pos)
    : super(pos)
    , m_text(text)
{
}

Label::Label(const Rect &pos)
    : Label("", pos)
{
}

Label::~Label()
{
}

void Label::setFontColor(FrameBuffer::Colors color)
{
  m_fontColor = color;
}

void Label::setFontColor(FrameBuffer &fb) const
{
  if(m_fontColor != FrameBuffer::Colors::Undefined)
  {
    fb.setColor(m_fontColor);
  }
  else if(isHighlight())
  {
    fb.setColor(FrameBuffer::Colors::C255);
  }
  else
  {
    fb.setColor(FrameBuffer::Colors::C128);
  }
}

void Label::setSuffixFontColor(FrameBuffer &fb) const
{
  setFontColor(fb);
}

bool Label::setText(const StringAndSuffix &text)
{
  if(m_text != text)
  {
    m_text = text;
    setDirty();
    return true;
  }
  return false;
}

Label::StringAndSuffix Label::getText() const
{
  return m_text;
}

Label::StringAndSuffix Label::shortenStringIfNeccessary(std::shared_ptr<Font> font,
                                                        const Label::StringAndSuffix &text) const
{
  return text;
}

bool Label::redraw(FrameBuffer &fb)
{
  auto pos = getPosition();
  auto font = getFont();
  auto clip = fb.clipRespectingOffset(pos);

  if(clip.isEmpty())
    return false;

  setFontColor(fb);

  int offset = (pos.getHeight() - getFontHeight()) / 2;

  auto text = getText();
  int width = font->getStringWidth(text.text);

  if(width >= (pos.getWidth() - getXOffset() - getRightMargin()))
  {
    text = shortenStringIfNeccessary(font, text);
    width = font->getStringWidth(text.text);
  }

  auto splits = text.getSplits();
  auto left = pos.getX() + getXOffset();

  switch(getJustification())
  {
    case Font::Justification::Center:
      font->draw(splits.first, left + (pos.getWidth() - width) / 2, pos.getBottom() - offset + getYOffset());
      break;

    case Font::Justification::Left:
      font->draw(splits.first, left, pos.getBottom() - offset + getYOffset());
      break;

    case Font::Justification::Right:
      font->draw(splits.first, left + (pos.getWidth() - width - getRightMargin()),
                 pos.getBottom() - offset + getYOffset());
      break;
  }

  if(!splits.second.empty())
  {
    drawSuffix(fb, width, splits.first, splits.second);
  }

  super::redraw(fb);
  return true;
}

void Label::drawSuffix(FrameBuffer &fb, int fullWidth, const Glib::ustring &firstPart, const Glib::ustring &secondPart)
{
  auto pos = getPosition();
  auto font = getFont();
  int offset = (pos.getHeight() - getFontHeight()) / 2;

  int firstPartWidth = font->getStringWidth(firstPart);

  setSuffixFontColor(fb);

  auto left = pos.getX() + getXOffset() + firstPartWidth;

  switch(getJustification())
  {
    case Font::Justification::Center:
      font->draw(secondPart, left + (pos.getWidth() - fullWidth) / 2, pos.getBottom() - offset + getYOffset());
      break;

    case Font::Justification::Left:
      font->draw(secondPart, left, pos.getBottom() - offset + getYOffset());
      break;

    case Font::Justification::Right:
      font->draw(secondPart, left + (pos.getWidth() - fullWidth), pos.getBottom() - offset + getYOffset());
      break;
  }
}

std::shared_ptr<Font> Label::getFont() const
{
  return Oleds::get().getFont("Emphase_9_Regular", getFontHeight());
}

int Label::getFontHeight() const
{
  return 9;
}

int Label::getXOffset() const
{
  return 0;
}

int Label::getYOffset() const
{
  return 0;
}

int Label::getRightMargin() const
{
  return 0;
}

Font::Justification Label::getJustification() const
{
  return Font::Justification::Center;
}
