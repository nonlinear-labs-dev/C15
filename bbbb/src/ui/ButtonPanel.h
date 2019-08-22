#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "ButtonColumn.h"

class ButtonPanel : public Gtk::HBox
{
 public:
  explicit ButtonPanel(int firstButtonId);

  void setLed(int idx, bool state);

 private:
  std::array<std::unique_ptr<ButtonColumn>, 6> m_columns;
};

#endif
