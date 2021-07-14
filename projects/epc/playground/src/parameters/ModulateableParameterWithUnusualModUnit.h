#pragma once

#include "ModulateableParameter.h"

class ModulateableParameterWithUnusualModUnit : public ModulateableParameter
{
 private:
  typedef ModulateableParameter super;

 public:
  ModulateableParameterWithUnusualModUnit(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling,
                                          const ScaleConverter *modAmountScaling, tDisplayValue def,
                                          int coarseDenominator, int fineDenominator);

  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  Glib::ustring stringizeModulationAmount() const override;
  Glib::ustring stringizeModulationAmount(tControlPositionValue amount) const override;
  double getModulationAmountFineDenominator() const override;
  double getModulationAmountCoarseDenominator() const override;

 private:
  const ScaleConverter *m_modAmountScaling;
};
