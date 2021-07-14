#pragma once

#include "proxies/hwui/controls/Button.h"
#include <sigc++/connection.h>

class Parameter;

class ModulationModeButton : public Button
{
 private:
  typedef Button super;

 public:
  ModulationModeButton(const Glib::ustring &caption, Buttons id);
  ~ModulationModeButton() override;

 protected:
  virtual void update(const Parameter *parameter) = 0;

  void onSoundTypeChanged();

 private:
  void onParameterSelectionChanged(Parameter *oldParameter, Parameter *newParameter);
  Parameter *m_param = nullptr;
  sigc::connection m_paramValueConnection;
};
