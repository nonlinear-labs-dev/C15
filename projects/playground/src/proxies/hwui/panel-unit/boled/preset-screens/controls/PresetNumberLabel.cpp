#include "PresetNumberLabel.h"
#include "Application.h"
#include <presets/Bank.h>
#include <presets/Preset.h>
#include "proxies/hwui/Oleds.h"
#include <proxies/hwui/FrameBuffer.h>
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
  if(showsLoadedPreset())
  {
    fb.setColor(FrameBufferColors::C103);
    int yinset = showsSelectedPreset() ? 2 : 1;
    int xinset = 2;
    const Rect &r = getPosition();
    fb.fillRect(r.getX() + xinset, r.getY() + yinset, r.getWidth() - xinset, r.getHeight() - 2 * yinset);
  }
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
    g_sprintf(txt, "%03d", pos);
    setText(txt, selected, loaded);
  }
}

std::shared_ptr<Font> PresetNumberLabel::getFont() const
{
  return Oleds::get().getFont("Emphase-9-NUM-Regular", getFontHeight());
}

int PresetNumberLabel::getXOffset() const
{
  return 0;
}
