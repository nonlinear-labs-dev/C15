#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <proxies/hwui/Oleds.h>

LeftAlignedLabel::LeftAlignedLabel(const Glib::ustring &text, const Rect &pos)
    : Label(text, pos)
{
}

shared_ptr<Font> LeftAlignedLabel::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int LeftAlignedLabel::getFontHeight() const
{
  return 8;
}

Font::Justification LeftAlignedLabel::getJustification() const
{
  return Font::Justification::Left;
}
