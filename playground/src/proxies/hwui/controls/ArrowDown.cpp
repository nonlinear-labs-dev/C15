#include "ArrowDown.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

ArrowDown::ArrowDown(const Rect &pos)
    : super(true, false, "w", pos)
{
}

ArrowDown::~ArrowDown()
{
}

shared_ptr<Font> ArrowDown::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Symbol", getFontHeight());
}
