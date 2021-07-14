#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundLabel.h>

class UpperModulationBoundLabel : public ModulationBoundLabel
{
  typedef ModulationBoundLabel super;

 public:
  explicit UpperModulationBoundLabel(const Rect &r);
  ~UpperModulationBoundLabel() override;

 protected:
  void updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam) override;
};
