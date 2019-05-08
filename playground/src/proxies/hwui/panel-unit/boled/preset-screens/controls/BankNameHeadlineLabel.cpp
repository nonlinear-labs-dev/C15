#include <proxies/hwui/Oleds.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/PresetBank.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankNameHeadlineLabel.h>

BankNameHeadlineLabel::BankNameHeadlineLabel(const Rect &pos)
    : super(pos)
{
  setText("Bank");
}

shared_ptr<Font> BankNameHeadlineLabel::getFont() const
{
  return Oleds::get().getFont("Emphase_9_Bold", getFontHeight());
}

int BankNameHeadlineLabel::getFontHeight() const
{
  return 9;
}

void BankNameHeadlineLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C255);
}

bool BankNameHeadlineLabel::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C77);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}
