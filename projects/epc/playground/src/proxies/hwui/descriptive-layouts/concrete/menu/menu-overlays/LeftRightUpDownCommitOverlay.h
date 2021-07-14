#pragma once
#include "MenuOverlay.h"

class LeftRightUpDownCommitOverlay : public MenuOverlay
{
 public:
  explicit LeftRightUpDownCommitOverlay(const Rect& r);
  bool onButton(Buttons i, bool down, ButtonModifiers mod) override;

  virtual void onLeft(bool down) = 0;
  virtual void onRight(bool down) = 0;
  virtual void onUp(bool down) = 0;
  virtual void onDown(bool down) = 0;
  virtual void onCommit(bool down) = 0;
};