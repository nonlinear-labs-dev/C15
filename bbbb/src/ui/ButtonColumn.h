#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "Button.h"
<<<<<<< HEAD
#include <array>

class ButtonColumn : public Gtk::VBox
{
 public:
  ButtonColumn(int firstButtonId);
  virtual ~ButtonColumn();

  void setLed(int idx, bool state);

 private:
  Button* m_buttons[4];
=======

class ButtonColumn : public Gtk::VBox
{
  public:
    ButtonColumn(int firstButtonId);
    virtual ~ButtonColumn();

    void setLed(int idx, bool state);

  private:
    std::array<Button, 4> m_buttons;
>>>>>>> layouts-reloaded
};

#endif
