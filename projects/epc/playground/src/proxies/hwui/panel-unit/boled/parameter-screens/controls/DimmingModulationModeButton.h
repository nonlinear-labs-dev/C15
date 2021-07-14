#pragma once

#include "ModulationModeButton.h"

class Application;

class DimmingModulationModeButton : public ModulationModeButton
{
 private:
  typedef ModulationModeButton super;

 public:
  DimmingModulationModeButton(const Glib::ustring &text, Buttons id);
  ~DimmingModulationModeButton() override;
};
