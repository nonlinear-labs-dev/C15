#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "Boled.h"
#include "Button.h"

class PlayPanel : public Gtk::VBox
{
  public:
    PlayPanel();
    virtual ~PlayPanel();

    void setFrameBuffer(WebSocketServer::tMessage msg);

  private:
    Button m_setup;
    Boled m_boled;
    Button m_preset;
    Button m_sound;
    Button m_a;
    Button m_b;
    Button m_c;
    Button m_d;
    Button m_store;

    Button m_info;
    Button m_fine;
    Button m_shift;
    Button m_default;
    Button m_inc;
    Button m_dec;
    Button m_enter;
    Button m_undo;
    Button m_edit;
    Button m_redo;

    Button m_minus;
    Button m_plus;
    Button m_function;
    Button m_mode;

    Gtk::HScale m_rotary;

    Gtk::VBox m_box;
    Gtk::FlowBox m_buttons;

};

#endif
