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
    std::array<ButtonColumn, 6> m_columns;
};

#endif
