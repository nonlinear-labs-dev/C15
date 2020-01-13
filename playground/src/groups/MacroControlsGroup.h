#pragma once

#include "ParameterGroup.h"
#include "parameters/ModulateableParameter.h"

class MacroControlsGroup : public ParameterGroup
{
 public:
  MacroControlsGroup(ParameterDualGroupSet *parent);
  virtual ~MacroControlsGroup();

  void init();

  static ParameterId modSrcToSmoothingId(MacroControls src);
  static ParameterId modSrcToParamId(MacroControls src);
  static MacroControls paramIDToModSrc(ParameterId pid);
  static bool isMacroTime(const ParameterId &id);
};
