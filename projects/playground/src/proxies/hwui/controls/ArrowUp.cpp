#include "ArrowUp.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"
#include <proxies/hwui/Oleds.h>

ArrowUp::ArrowUp(const Rect &pos)
    : super(true, false, "\u25B2", pos)
{
}

ArrowUp::~ArrowUp()
{
}

std::shared_ptr<Font> ArrowUp::getFont() const
{
  return Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
}
