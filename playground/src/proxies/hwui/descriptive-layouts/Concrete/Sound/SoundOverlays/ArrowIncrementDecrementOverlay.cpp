#include "ArrowIncrementDecrementOverlay.h"

ArrowIncrementDecrementOverlay::ArrowIncrementDecrementOverlay(const Rect &r) : ScrollMenuOverlay(r) {

  auto fullWidth = r.getWidth();
  auto labelWidth = fullWidth - 20;

  m_leftArrow = addControl(new ArrowLeft({0, -2, 10, 13}));
  m_rightArrow = addControl(new ArrowRight({labelWidth + 10, -2, 10, 13}));
}

bool ArrowIncrementDecrementOverlay::onButton(Buttons i, bool down, ButtonModifiers mod) {
  if(i == Buttons::BUTTON_C)
  {
    m_leftArrow->setHighlight(down);
    onLeft(down);
    return true;
  }
  if(i == Buttons::BUTTON_D)
  {
    m_rightArrow->setHighlight(down);
    onRight(down);
    return true;
  }
  if(i == Buttons::BUTTON_ENTER)
  {
    onCommit(down);
    return true;
  }
  return false;
}