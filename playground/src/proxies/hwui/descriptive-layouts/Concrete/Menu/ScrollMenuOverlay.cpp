#include "ScrollMenuOverlay.h"

bool ScrollMenuOverlay::onButton(Buttons i, bool down, ButtonModifiers mod) {
  return false;
}

ScrollMenuOverlay::ScrollMenuOverlay(const Rect &r) : ControlWithChildren(r) {

}
