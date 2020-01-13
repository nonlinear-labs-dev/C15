#pragma once
#include "ModulateableParameterWithUnusualModUnit.h"

class SplitPointParameter : public ModulateableParameterWithUnusualModUnit
{
 public:
  SplitPointParameter(ParameterGroup *group, const ParameterId &id);
  std::string getDisplayValue(VoiceGroup vg) const;

  DFBLayout *createLayout(FocusAndMode focusAndMode) const override;
  Glib::ustring getDisplayString() const override;

  ustring stringizeModulationAmount(tControlPositionValue amount) const override;

 protected:
  ustring modulationValueToDisplayString(tControlPositionValue v) const override;
};
