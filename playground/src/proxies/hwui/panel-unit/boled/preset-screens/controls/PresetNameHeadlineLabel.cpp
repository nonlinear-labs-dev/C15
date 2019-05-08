#include <proxies/hwui/Oleds.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/PresetBank.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetNameHeadlineLabel.h>

PresetNameHeadlineLabel::PresetNameHeadlineLabel(const Rect &pos)
    : super(pos)
{
  setText("Preset");
}

shared_ptr<Font> PresetNameHeadlineLabel::getFont() const
{
  return Oleds::get().getFont("Emphase_9_Bold", getFontHeight());
}

int PresetNameHeadlineLabel::getFontHeight() const
{
  return 9;
}

void PresetNameHeadlineLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C255);
}

bool PresetNameHeadlineLabel::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C77);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}
