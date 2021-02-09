#pragma once

#include "ModulationBoundControl.h"

class LowerModulationBoundControl : public ModulationBoundControl
{
  typedef ModulationBoundControl super;

 public:
  explicit LowerModulationBoundControl(const Rect &r);

  bool onRotary(int inc, ButtonModifiers modifiers) override;

 private:
  void onSelectionChanged(Parameter *, Parameter *newParam, SignalOrigin signalType);
  void onParameterChanged(const Parameter *p);

  sigc::connection m_paramChanged;
};
