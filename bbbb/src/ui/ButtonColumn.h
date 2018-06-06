#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "Button.h"

class ButtonColumn : public Gtk::VBox
{
  public:
    ButtonColumn(int firstButtonId);
    virtual ~ButtonColumn();

    void setLed(int idx, bool state);

  private:
    std::array<Button, 4> m_buttons;
};

#endif
