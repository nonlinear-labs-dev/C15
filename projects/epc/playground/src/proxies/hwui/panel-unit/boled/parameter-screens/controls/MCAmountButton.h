#pragma once

#include "DimmingModulationModeButton.h"

class Application;

class MCAmountButton : public DimmingModulationModeButton
{
 private:
  typedef DimmingModulationModeButton super;

 public:
  explicit MCAmountButton(Buttons id);
  ~MCAmountButton() override;

  void update(const Parameter *parameter) override;
};
