#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>

class UpperModulationBoundControl : public ControlWithChildren, public RotaryEncoder::Receiver
{
  typedef ControlWithChildren super;

 public:
  UpperModulationBoundControl(const Rect &r);
  virtual ~UpperModulationBoundControl();

  bool onRotary(int inc, ButtonModifiers modifiers) override;
};
