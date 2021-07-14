#pragma once
#include <tools/RecursionGuard.h>
#include "ModulateableParameterWithUnusualModUnit.h"

class Setting;
class Preset;

/*
 * The SplitPointParameter of a VoiceGroup is the upper (VGI) or
 * the lower (VGII) end of the according key range. The respective
 * 'other' end is 0 (VG1) or 60 (VGII). The range is meant to include
 * both ends:
 * 
 * VGI : [0 ... SplitPointVGI]
 * VGII: [SplitPointVGII ... 60]
 * 
 * 0 is the lowest key on the C15 keyboard, C1
 * 60 is the highest key on the C15 keyboard, C6
 * 
 * Note: If SplitPointSync-Setting is enabled, then 
 *       SplitPointVGI has to be <= 59
 *       SplitPointVGII has to be >= 1
 * 
 * For example: 
 * 
 * SplitPoint Parameter | CP    |  VG I   |   VG II   
 *    Key Numbers       |       |         |           
 * --------------------------------------------------
 *                   0  |  0    |   C1    |    C1-C6   
 *                   1  | 1/60  | C1-Cis1 |   Cis1-C6   
 *                  12  | 12/60 |  C1-C2  |    C2-C6   
 *                  60  |  1    |  C1-C6  |     C6   
 */

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
  SplitPointParameter* getSibling() const;
  bool hasOverlap();
  bool inDefaultSplitBehaviour() const;
  void updateCPFromSyncChange(UNDO::Transaction* transaction, double cp);
  void loadDefault(UNDO::Transaction* transaction, Defaults mode) override;

 protected:
  void setCpValue(UNDO::Transaction* transaction, Initiator initiator, tControlPositionValue value,
                  bool dosendToPlaycontroller) override;

  bool inModAmountSet = false;
  bool inModSrcSet = false;
  void preventNegativeOverlap(UNDO::Transaction* transaction, tControlPositionValue value, bool dosendToPlaycontroller);
  bool isAtExtremes(tControlPositionValue value);

  void clampToExtremes(UNDO::Transaction* transaction, bool dosendToPlaycontroller);
};
