#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>
#include <proxies/hwui/panel-unit/ButtonReceiver.h>

class Parameter;

class ModulationBoundControl : public ControlWithChildren, public RotaryEncoder::Receiver, public ButtonReceiver
{
 public:
  using ControlWithChildren::ControlWithChildren;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};
