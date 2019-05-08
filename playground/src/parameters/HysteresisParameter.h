#pragma once

#include <parameters/PhysicalControlParameter.h>
#include "value/Hysteresis.h"

class HysteresisParameter : public PhysicalControlParameter
{
 private:
  typedef PhysicalControlParameter super;

 public:
  HysteresisParameter(ParameterGroup *group, uint16_t id, const ScaleConverter *scaling, tDisplayValue def,
                      int coarseDenominator, int fineDenominator);

  virtual void onChangeFromLpc(tControlPositionValue newValue) override;

 private:
  Hysteresis m_lpcHysteresis;
};
