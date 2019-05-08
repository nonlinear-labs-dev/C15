#include "ArrowUp.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

ArrowUp::ArrowUp(const Rect &pos)
    : super(true, false, "q", pos)
{
}

ArrowUp::~ArrowUp()
{
}

shared_ptr<Font> ArrowUp::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Symbol", getFontHeight());
}
