#pragma once

#include "ModulationModeButton.h"

class Application;

class MCSelectButton : public ModulationModeButton
{
 private:
  typedef ModulationModeButton super;

 public:
  explicit MCSelectButton(Buttons id);
  ~MCSelectButton() override;

 protected:
  void update(const Parameter *parameter) override;
};
