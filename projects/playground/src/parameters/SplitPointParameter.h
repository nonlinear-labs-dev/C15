#pragma once
#include "ModulateableParameterWithUnusualModUnit.h"

class SplitPointParameter : public ModulateableParameterWithUnusualModUnit
{
 public:
  SplitPointParameter(ParameterGroup *group, const ParameterId &id);
  Glib::ustring getDisplayValue(VoiceGroup vg) const;

  Layout *createLayout(FocusAndMode focusAndMode) const override;
  Glib::ustring getDisplayString() const override;
  Glib::ustring stringizeModulationAmount(tControlPositionValue amount) const override;

 protected:
  Glib::ustring modulationValueToDisplayString(tControlPositionValue v) const override;
};
