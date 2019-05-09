#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "ButtonColumn.h"

class ButtonPanel : public Gtk::HBox
{
 public:
  ButtonPanel(int firstButtonId);
  virtual ~ButtonPanel();

  void setLed(int idx, bool state);

 private:
  ButtonColumn* m_columns[6];
};

#endif
