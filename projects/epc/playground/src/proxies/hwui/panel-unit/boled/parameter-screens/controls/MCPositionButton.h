#pragma once

#include "DimmingModulationModeButton.h"

class Application;

class MCPositionButton : public ModulationModeButton
{
 private:
  typedef ModulationModeButton super;

 public:
  explicit MCPositionButton(Buttons id);
  ~MCPositionButton() override;

  void update(const Parameter *parameter) override;
};
