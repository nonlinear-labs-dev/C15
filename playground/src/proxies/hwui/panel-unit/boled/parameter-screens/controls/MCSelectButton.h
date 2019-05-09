#pragma once

#include "ModulationModeButton.h"

class Application;

class MCSelectButton : public ModulationModeButton
{
 private:
  typedef ModulationModeButton super;

 public:
  MCSelectButton(Buttons id);
  virtual ~MCSelectButton();

 protected:
  virtual void update(const Parameter *parameter) override;
};
