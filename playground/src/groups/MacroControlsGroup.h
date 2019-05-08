#pragma once

#include "ParameterGroup.h"
#include "parameters/ModulateableParameter.h"

class MacroControlsGroup : public ParameterGroup
{
 public:
  MacroControlsGroup(ParameterGroupSet *parent);
  virtual ~MacroControlsGroup();

  void init();

  static uint16_t modSrcToParamID(ModulateableParameter::ModulationSource src);
  static ModulateableParameter::ModulationSource paramIDToModSrc(uint16_t pid);
};
