#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundLabel.h>

class UpperModulationBoundLabel : public ModulationBoundLabel
{
  typedef ModulationBoundLabel super;

 public:
  UpperModulationBoundLabel(const Rect &r);
  virtual ~UpperModulationBoundLabel();

 protected:
  void updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam) override;
};
