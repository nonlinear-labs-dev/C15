#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>

class Button : public Gtk::Button
{
  public:
    Button(int id, const std::string &title = "");
    virtual ~Button();

    void setLed(int idx, bool state);

  protected:
    bool on_draw (const ::Cairo::RefPtr< ::Cairo::Context >& cr) override;
    void on_pressed() override;
    void on_released() override;


  private:
    int m_buttonId;
    bool m_state = false;
};

#endif
