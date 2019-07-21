#pragma once

#include "ParameterGroup.h"
#include "parameters/ModulateableParameter.h"

class MacroControlsGroup : public ParameterGroup
{
 public:
  MacroControlsGroup(ParameterGroupSet *parent);
  virtual ~MacroControlsGroup();

  void init();

  static uint16_t modSrcToParamID(MacroControls src);
  static MacroControls paramIDToModSrc(uint16_t pid);
};
