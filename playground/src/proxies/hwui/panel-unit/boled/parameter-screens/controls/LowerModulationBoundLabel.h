#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundLabel.h>

class LowerModulationBoundLabel : public ModulationBoundLabel
{
  typedef ModulationBoundLabel super;

 public:
  LowerModulationBoundLabel(const Rect &r);
  virtual ~LowerModulationBoundLabel();

 protected:
  void updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam) override;
};
