#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/MenuOverlay/ScrollMenuOverlay.h>
#include <proxies/hwui/controls/ArrowRight.h>
#include <proxies/hwui/controls/ArrowLeft.h>
#include <proxies/hwui/controls/LabelRegular8.h>

class ArrowIncrementDecrementOverlay : public ScrollMenuOverlay {
public:
  explicit ArrowIncrementDecrementOverlay(const Rect& r);

  bool onButton(Buttons i, bool down, ButtonModifiers mod) override;

  virtual void onLeft(bool down) = 0;
  virtual void onRight(bool down) = 0;
  virtual void onCommit(bool down) = 0;

protected:
  ArrowRight* m_rightArrow;
  ArrowLeft* m_leftArrow;
};
