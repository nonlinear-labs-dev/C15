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

  virtual void undoableIncrementMCAmount(UNDO::Transaction *transaction, int inc, ButtonModifiers modifiers) override;
  virtual void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  virtual Glib::ustring stringizeModulationAmount() const override;
  virtual Glib::ustring stringizeModulationAmount(tControlPositionValue amount) const override;
  virtual double getModulationAmountFineDenominator() const override;
  virtual double getModulationAmountCoarseDenominator() const override;

 private:
  const ScaleConverter *m_modAmountScaling;
};
