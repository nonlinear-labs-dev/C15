#include "LeftRightUpDownCommitOverlay.h"

LeftRightUpDownCommitOverlay::LeftRightUpDownCommitOverlay(const Rect& r)
    : MenuOverlay { r }
{
}

bool LeftRightUpDownCommitOverlay::onButton(Buttons i, bool down, ButtonModifiers mod)
{
  switch(i)
  {
    case Buttons::BUTTON_ENTER:
      onCommit(down);
      break;
    case Buttons::BUTTON_DEC:
    case Buttons::BUTTON_C:
    case Buttons::ROTARY_MINUS:
      onLeft(down);
      break;
    case Buttons::BUTTON_INC:
    case Buttons::BUTTON_D:
    case Buttons::ROTARY_PLUS:
      onRight(down);
      break;
    default:
      return false;
  }
  return true;
}