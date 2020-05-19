#pragma once
#include "Parameter.h"

class MacroControlSmoothingParameter : public Parameter
{
 public:
  using Parameter::Parameter;
  Layout *createLayout(FocusAndMode focusAndMode) const override;

 public:
  ParameterId getMC() const;
};