#include "ArrowDown.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"
#include <proxies/hwui/Oleds.h>

ArrowDown::ArrowDown(const Rect &pos)
    : super(true, false, "\u25BC", pos)
{
}

ArrowDown::~ArrowDown() = default;

std::shared_ptr<Font> ArrowDown::getFont() const
{
  return Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
}
