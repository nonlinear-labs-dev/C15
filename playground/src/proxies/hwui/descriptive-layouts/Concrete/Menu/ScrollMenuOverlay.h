#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/ButtonReceiver.h>

class ScrollMenuOverlay : public ControlWithChildren, public ButtonReceiver {
public:
  explicit ScrollMenuOverlay(const Rect& r);
  bool onButton(Buttons i, bool down, ButtonModifiers mod) override;
};
