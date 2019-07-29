#pragma once

#include "ModulationBoundControl.h"

class LowerModulationBoundControl : public ModulationBoundControl
{
  typedef ModulationBoundControl super;

 public:
  explicit LowerModulationBoundControl(const Rect &r);

  bool onRotary(int inc, ButtonModifiers modifiers) override;

 private:
  void onSelectionChanged(Parameter *, Parameter *newParam);
  void onParameterChanged(const Parameter *p);

  connection m_paramChanged;
};
