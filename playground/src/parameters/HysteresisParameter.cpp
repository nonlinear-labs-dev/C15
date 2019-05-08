#include "HysteresisParameter.h"

HysteresisParameter::HysteresisParameter(ParameterGroup *group, uint16_t id, const ScaleConverter *scaling,
                                         tDisplayValue def, int coarseDenominator, int fineDenominator)
    : super(group, id, scaling, def, coarseDenominator, fineDenominator)
    , m_lpcHysteresis(def, 1.0 / 100.0)
{
}

void HysteresisParameter::onChangeFromLpc(tControlPositionValue newVal)
{
  double smoothed = m_lpcHysteresis.set(newVal);

  if(getValue().differs(smoothed))
  {
    super::onChangeFromLpc(smoothed);
  }
}
