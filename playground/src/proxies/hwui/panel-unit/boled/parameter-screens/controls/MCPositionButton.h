#pragma once

#include "DimmingModulationModeButton.h"

class Application;

class MCPositionButton : public ModulationModeButton
{
 private:
  typedef ModulationModeButton super;

 public:
  MCPositionButton(Buttons id);
  virtual ~MCPositionButton();

  void update(const Parameter *parameter);
};
