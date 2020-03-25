#include "SymbolLabel.h"
#include "proxies/hwui/Oleds.h"

SymbolLabel::SymbolLabel(const Glib::ustring &text, const Rect &pos)
    : super(text, pos)
{
}

SymbolLabel::~SymbolLabel()
{
}

std::shared_ptr<Font> SymbolLabel::getFont() const
{
  return Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
}

int SymbolLabel::getFontHeight() const
{
  return 8;
}
