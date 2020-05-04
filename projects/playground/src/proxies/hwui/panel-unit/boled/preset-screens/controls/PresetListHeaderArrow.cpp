#include "PresetListHeaderArrow.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include <presets/Bank.h>
#include "proxies/hwui/Oleds.h"
#include <proxies/hwui/FrameBuffer.h>

PresetListHeaderArrow::PresetListHeaderArrow(const Glib::ustring &text, const Rect &pos, const int xOffset)
    : super(text, pos)
    , m_xOffset { xOffset }
{
}

PresetListHeaderArrow::~PresetListHeaderArrow()
{
}

void PresetListHeaderArrow::setBackgroundColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBufferColors::C255);
  else
    fb.setColor(FrameBufferColors::C77);
}

int PresetListHeaderArrow::getYOffset() const
{
  return -1;
}

int PresetListHeaderArrow::getXOffset() const
{
  return m_xOffset;
}

void PresetListHeaderArrow::setFontColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBufferColors::C103);
  else
    fb.setColor(FrameBufferColors::C255);
}

bool PresetListHeaderArrow::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  if(isHighlight())
    fb.setColor(FrameBufferColors::C255);
  else
    fb.setColor(FrameBufferColors::C103);

  fb.drawRect(getPosition());
  return true;
}

Font::Justification PresetListHeaderArrow::getJustification() const
{
  return Font::Justification::Center;
}

std::shared_ptr<Font> PresetListHeaderArrow::getFont() const
{
  return Oleds::get().getFont("Emphase-9-Regular", getFontHeight());
}
