#pragma once

#include "PhysicalControlParameter.h"

class PitchbendParameter : public PhysicalControlParameter
{
 private:
  typedef PhysicalControlParameter super;

 public:
  using super::super;

  ReturnMode getReturnMode() const override;

 protected:
  Layout *createLayout(FocusAndMode focusAndMode) const override;
};
