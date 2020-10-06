#pragma once
#include "ModulateableParameterWithUnusualModUnit.h"

class SplitPointParameter : public ModulateableParameterWithUnusualModUnit
{
 public:
  SplitPointParameter(ParameterGroup* group, const ParameterId& id);

  Layout* createLayout(FocusAndMode focusAndMode) const override;
  Glib::ustring stringizeModulationAmount(tControlPositionValue amount) const override;
  void setModulationAmount(UNDO::Transaction* transaction, const tDisplayValue& amount) override;
  void setModulationAmountFromSibling(UNDO::Transaction* transaction, const tDisplayValue& amount);
  void setModulationSource(UNDO::Transaction* transaction, MacroControls src) override;
  void setModulationSourceFromSibling(UNDO::Transaction* transaction, MacroControls src);

 protected:
  SplitPointParameter* getSibling() const;

  void setCpValue(UNDO::Transaction* transaction, Initiator initiator, tControlPositionValue value,
                  bool dosendToPlaycontroller) override;

  bool inModAmountSet = false;
  bool inModSrcSet = false;
};
