#include "LabelRegular8.h"
#include "proxies/hwui/Oleds.h"

LabelRegular8::LabelRegular8(const Glib::ustring &text, const Rect &pos)
    : super(text, pos)
{
}

LabelRegular8::LabelRegular8(const Rect &pos)
    : LabelRegular8("", pos)
{
}

LabelRegular8::~LabelRegular8()
{
}

shared_ptr<Font> LabelRegular8::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int LabelRegular8::getFontHeight() const
{
  return 8;
}
