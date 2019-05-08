#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundLabel.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>

class MacroControlParameter;
class ModulateableParameter;

class CurrentModulatedValueLabel : public ModulationBoundLabel, public RotaryEncoder::Receiver
{
  typedef ModulationBoundLabel super;

 public:
  CurrentModulatedValueLabel(const Rect &r);
  virtual ~CurrentModulatedValueLabel();

  bool onRotary(int inc, ButtonModifiers modifiers) override;

 protected:
  void updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam) override;
};
