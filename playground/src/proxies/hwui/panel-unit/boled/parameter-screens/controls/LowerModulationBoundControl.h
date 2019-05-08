#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>

class LowerModulationBoundControl : public ControlWithChildren, public RotaryEncoder::Receiver
{
  typedef ControlWithChildren super;

 public:
  LowerModulationBoundControl(const Rect &r);
  virtual ~LowerModulationBoundControl();

  bool onRotary(int inc, ButtonModifiers modifiers) override;

 private:
  void onSelectionChanged(Parameter *, Parameter *newParam);
  void onParameterChanged(const Parameter *p);

  Control *m_dummyButton = nullptr;
  connection m_paramChanged;
};
