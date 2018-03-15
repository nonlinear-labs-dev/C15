#include "PresetNameLabel.h"
#include "Application.h"
#include "presets/Preset.h"

PresetNameLabel::PresetNameLabel (const Rect &pos) :
    super (pos)
{
}

PresetNameLabel::~PresetNameLabel ()
{
}

void PresetNameLabel::update(const Glib::ustring &presetName, bool selected, bool loaded)
{
  setText(presetName, selected, loaded);
}

void PresetNameLabel::drawBackground (FrameBuffer &fb)
{
  super::drawBackground (fb);

  const Rect &r = getPosition ();

  if (showsLoadedPreset())
    fb.setColor (FrameBuffer::Colors::C103);
  else
    fb.setColor (FrameBuffer::Colors::C43);

  int xinset = showsSelectedPreset() ? 3 : 1;
  int yinset = showsSelectedPreset() ? 2 : 1;

  fb.fillRect (r.getX (), r.getY () + yinset, r.getWidth () - xinset, r.getHeight () - 2 * yinset);
}

Font::Justification PresetNameLabel::getJustification () const
{
  return Font::Justification::Left;
}

Glib::ustring PresetNameLabel::shortenStringIfNeccessary (shared_ptr<Font> font, const Glib::ustring &text) const
{
  auto width = getPosition().getWidth() - getXOffset() - getRightMargin();
  auto c = text;

  for(size_t len = text.length(); len > 0 && (font->getStringWidth(c) >= width); len--)
  {
    c = text.substr(0, len) + "..";
  }

  return c;
}

int PresetNameLabel::getXOffset () const
{
  return 7;
}

int PresetNameLabel::getRightMargin() const
{
  return 3;
}
