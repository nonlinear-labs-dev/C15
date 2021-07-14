#include "Label.h"
#include "proxies/hwui/Oleds.h"
#include <proxies/hwui/FrameBuffer.h>

#include <utility>

Label::Label(StringAndSuffix text, const Rect &pos)
    : super(pos)
    , m_text(std::move(text))
{
}

Label::Label(const Rect &pos)
    : Label(StringAndSuffix { "" }, pos)
{
}

Label::~Label() = default;

void Label::setFontColor(FrameBufferColors color)
{
  m_fontColor = color;
}

FrameBufferColors Label::getFontColor() const
{
  return m_fontColor;
}

void Label::setFontColor(FrameBuffer &fb) const
{
  if(m_fontColor != FrameBufferColors::Undefined)
  {
    fb.setColor(m_fontColor);
  }
  else if(isHighlight())
  {
    fb.setColor(FrameBufferColors::C255);
  }
  else
  {
    fb.setColor(FrameBufferColors::C128);
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

StringAndSuffix Label::getText() const
{
  return m_text;
}

StringAndSuffix Label::shortenStringIfNeccessary(const std::shared_ptr<Font> &font, const StringAndSuffix &text) const
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
  return Oleds::get().getFont("Emphase-9-Regular", getFontHeight());
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

void Label::setJustification(const Font::Justification &justification)
{
  m_justification = justification;
}

Font::Justification Label::getJustification() const
{
  return m_justification;
}