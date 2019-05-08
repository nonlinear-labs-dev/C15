#include "PresetNumberLabel.h"
#include "Application.h"
#include "presets/PresetBank.h"
#include "presets/Preset.h"
#include "proxies/hwui/Oleds.h"

#include <glib/gprintf.h>

PresetNumberLabel::PresetNumberLabel(const Rect &pos)
    : super(pos)
{
}

PresetNumberLabel::~PresetNumberLabel()
{
}

void PresetNumberLabel::drawBackground(FrameBuffer &fb)
{
  super::drawBackground(fb);

  if(showsLoadedPreset())
    fb.setColor(FrameBuffer::Colors::C103);
  else
    fb.setColor(FrameBuffer::Colors::C43);

  int xinset = showsSelectedPreset() ? 3 : 1;
  int yinset = showsSelectedPreset() ? 2 : 1;
  const Rect &r = getPosition();
  fb.fillRect(r.getX() + xinset, r.getY() + yinset, r.getWidth() - xinset, r.getHeight() - 2 * yinset);
}

Font::Justification PresetNumberLabel::getJustification() const
{
  return Font::Justification::Right;
}

void PresetNumberLabel::update(int presetNumber, bool selected, bool loaded)
{
  if(presetNumber == -1)
  {
    setText("", false, false);
  }
  else
  {
    auto pos = presetNumber + 1;
    char txt[256];
    g_sprintf(txt, "%03zu", pos);
    setText(txt, selected, loaded);
  }
}

shared_ptr<Font> PresetNumberLabel::getFont() const
{
  return Oleds::get().getFont("Emphase_9_NUM_Regular", getFontHeight());
}

int PresetNumberLabel::getXOffset() const
{
  return 0;
}
