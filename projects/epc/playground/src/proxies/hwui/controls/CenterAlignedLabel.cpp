#include "CenterAlignedLabel.h"

CenterAlignedLabel::CenterAlignedLabel(const Glib::ustring &text, const Rect &r)
    : LeftAlignedLabel(text, r)
{
}

Font::Justification CenterAlignedLabel::getJustification() const
{
  return Font::Justification::Center;
}
