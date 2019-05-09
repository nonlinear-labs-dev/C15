#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "Button.h"
#include <array>

class ButtonColumn : public Gtk::VBox
{
 public:
  ButtonColumn(int firstButtonId);
  virtual ~ButtonColumn();

  void setLed(int idx, bool state);

 private:
  Button* m_buttons[4];
};

#endif
