#include "SymbolLabel.h"
#include "proxies/hwui/Oleds.h"

SymbolLabel::SymbolLabel(const Glib::ustring &text, const Rect &pos)
    : super(text, pos)
{
}

SymbolLabel::~SymbolLabel()
{
}

shared_ptr<Font> SymbolLabel::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Symbol", getFontHeight());
}

int SymbolLabel::getFontHeight() const
{
  return 8;
}
