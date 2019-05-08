#pragma once

#include "DimmingModulationModeButton.h"

class Application;

class MCAmountButton : public DimmingModulationModeButton
{
 private:
  typedef DimmingModulationModeButton super;

 public:
  MCAmountButton(Buttons id);
  virtual ~MCAmountButton();

  void update(const Parameter *parameter) override;
};
