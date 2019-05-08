#include "BankNameLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/PresetBank.h"

BankNameLabel::BankNameLabel(const Rect &pos)
    : super(pos)
{
}

BankNameLabel::~BankNameLabel()
{
}

void BankNameLabel::updateLabel(shared_ptr<PresetBank> newBank)
{
  if(newBank)
  {
    Glib::ustring bankName = newBank->getName(true);
    Glib::ustring s
        = to_string(Application::get().getPresetManager()->calcOrderNumber(newBank.get())) + ": " + bankName;
    setText(s);
  }
  else
  {
    setText("");
  }
}

Glib::ustring BankNameLabel::shortenStringIfNeccessary(shared_ptr<Font> font, const Glib::ustring &text) const
{
  auto c = text;
  auto availableWidth = getPosition().getWidth() - 2 * getXOffset();

  for(size_t len = text.length(); len > 0 && (font->getStringWidth(c) > availableWidth); len--)
  {
    c = text.substr(0, len) + "..";
  }

  return c;
}

void BankNameLabel::setBackgroundColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C255);
  else
    fb.setColor(FrameBuffer::Colors::C103);
}

void BankNameLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

Font::Justification BankNameLabel::getJustification() const
{
  return Font::Justification::Center;
}

int BankNameLabel::getXOffset() const
{
  return 2;
}
