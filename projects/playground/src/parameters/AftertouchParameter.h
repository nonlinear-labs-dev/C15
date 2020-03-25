#pragma once

#include "PhysicalControlParameter.h"

class AftertouchParameter : public PhysicalControlParameter
{
 private:
  typedef PhysicalControlParameter super;

 public:
  using super::super;

  virtual ReturnMode getReturnMode() const override;

 protected:
  virtual Layout *createLayout(FocusAndMode focusAndMode) const override;
};
