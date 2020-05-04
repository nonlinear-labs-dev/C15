#pragma once
#include "UnmodulatebaleParameterLayouts.h"

class MacroControlSmoothingParameterLayout : public UnmodulateableParameterSelectLayout2
{
 public:
  using UnmodulateableParameterSelectLayout2::UnmodulateableParameterSelectLayout2;

 protected:
  void init() override;

 protected:
  bool m_modResetActive = false;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  void resetMod();
  void selectMacroControl();
};