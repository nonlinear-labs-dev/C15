#include <proxies/hwui/TextCropper.h>
#include "PresetNameLabel.h"
#include "Application.h"
#include <presets/Preset.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/HWUI.h>

PresetNameLabel::PresetNameLabel(const Rect &pos)
    : super(pos)
{
}

void PresetNameLabel::update(const Glib::ustring &presetName, bool selected, bool loaded)
{
  setText(presetName, selected, loaded);
}

void PresetNameLabel::drawBackground(FrameBuffer &fb)
{
  super::drawBackground(fb);

  const Rect &r = getPosition();

  if(showsLoadedPreset())
  {
    fb.setColor(FrameBufferColors::C103);
    int yinset = showsSelectedPreset() ? 2 : 1;
    fb.fillRect(r.getX(), r.getY() + yinset, r.getWidth(), r.getHeight() - 2 * yinset);
  }
}

Font::Justification PresetNameLabel::getJustification() const
{
  return Font::Justification::Left;
}

StringAndSuffix PresetNameLabel::shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                                           const StringAndSuffix &text) const
{
  return TextCropper::shortenStringIfNeccessary(font, text.text, getPosition().getWidth() - getXOffset());
}

int PresetNameLabel::getXOffset() const
{
  return 3;
}

int PresetNameLabel::getRightMargin() const
{
  return 0;
}
