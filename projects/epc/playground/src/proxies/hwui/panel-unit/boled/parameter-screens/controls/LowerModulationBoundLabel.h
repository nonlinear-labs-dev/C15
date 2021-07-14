#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundLabel.h>

class LowerModulationBoundLabel : public ModulationBoundLabel
{
  typedef ModulationBoundLabel super;

 public:
  explicit LowerModulationBoundLabel(const Rect &r);
  ~LowerModulationBoundLabel() override;

 protected:
  void updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam) override;
};
