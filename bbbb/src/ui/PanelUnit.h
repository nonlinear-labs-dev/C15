#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "ButtonPanel.h"

class PanelUnit : public Gtk::HBox
{
 public:
  PanelUnit();
  ~PanelUnit() override = default;

  void setLed(int idx, bool state);

 private:
  std::vector<std::unique_ptr<ButtonPanel>> m_panels;
};

#endif
