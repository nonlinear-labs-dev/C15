#pragma once

#include "proxies/hwui/controls/Button.h"

class Parameter;

class ModulationModeButton : public Button
{
 private:
  typedef Button super;

 public:
  ModulationModeButton(const Glib::ustring &caption, Buttons id);

  virtual ~ModulationModeButton();

 protected:
  virtual void update(const Parameter *parameter) = 0;

 private:
  void onParameterSelectionChanged(Parameter *oldParameter, Parameter *newParameter);

  sigc::connection m_paramValueConnection;
};
