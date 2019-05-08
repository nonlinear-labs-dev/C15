#include "PresetListHeaderArrow.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/PresetBank.h"
#include "proxies/hwui/Oleds.h"

PresetListHeaderArrow::PresetListHeaderArrow(const Glib::ustring &text, const Rect &pos)
    : super(text, pos)
{
}

PresetListHeaderArrow::~PresetListHeaderArrow()
{
}

void PresetListHeaderArrow::setBackgroundColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C255);
  else
    fb.setColor(FrameBuffer::Colors::C77);
}

void PresetListHeaderArrow::setFontColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C103);
  else
    fb.setColor(FrameBuffer::Colors::C255);
}

bool PresetListHeaderArrow::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C255);
  else
    fb.setColor(FrameBuffer::Colors::C103);

  fb.drawRect(getPosition());
  return true;
}

Font::Justification PresetListHeaderArrow::getJustification() const
{
  return Font::Justification::Center;
}

shared_ptr<Font> PresetListHeaderArrow::getFont() const
{
  return Oleds::get().getFont("Emphase_9_Symbol", getFontHeight());
}
