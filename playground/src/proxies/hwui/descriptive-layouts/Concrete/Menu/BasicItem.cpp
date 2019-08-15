#include "BasicItem.h"

bool BasicItem::canEnter() {
  return false;
}

void BasicItem::redraw(FrameBuffer &fb, int offset) {
  ControlWithChildren::redraw(fb);
}

int BasicItem::getOrderNumber() const {
  return m_orderNumber;
}
