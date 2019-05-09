#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "ButtonPanel.h"

class PanelUnit : public Gtk::HBox
{
<<<<<<< HEAD
 public:
  PanelUnit();
  virtual ~PanelUnit();

  void setLed(int idx, bool state);

 private:
  ButtonPanel* m_panels[4];
=======
  public:
    PanelUnit();
    virtual ~PanelUnit();

    void setLed(int idx, bool state);

  private:
    std::array<ButtonPanel, 4> m_panels;
>>>>>>> layouts-reloaded
};

#endif
