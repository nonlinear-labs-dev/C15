#include "PlayPanel.h"
#include <io/FromEncoderBridge.h>
#include <Application.h>
#include <io/Bridges.h>
#include <nltools/logging/Log.h>

enum Buttons
{
  BUTTON_SETUP = 96,
  BUTTON_SOUND = 97,

  BUTTON_A = 98,
  BUTTON_B = 99,
  BUTTON_C = 100,
  BUTTON_D = 101,

  BUTTON_STORE = 102,
  BUTTON_PRESET = 103,
  BUTTON_INFO = 104,
  BUTTON_FINE = 105,
  BUTTON_ENTER = 106,
  BUTTON_EDIT = 107,
  BUTTON_SHIFT = 108,
  BUTTON_DEFAULT = 109,
  BUTTON_DEC = 110,
  BUTTON_INC = 111,
  BUTTON_UNDO = 112,
  BUTTON_REDO = 113,

  BUTTON_MINUS = 120,
  BUTTON_PLUS = 121,
  BUTTON_FUNCTION = 122,
  BUTTON_MODE = 123,
};

PlayPanel::PlayPanel()
    : m_setup(BUTTON_SETUP, "Setup")
    , m_exportBoled("Export Boled", std::bind(&Boled::exportBoled, &m_boled))
    , m_exportSoled("Export Soled", std::bind(&Boled::exportSoled, &m_boled))
    , m_preset(BUTTON_PRESET, "Preset")
    , m_sound(BUTTON_SOUND, "Sound")
    , m_store(BUTTON_STORE, "Store")
    , m_a(BUTTON_A, "A")
    , m_b(BUTTON_B, "B")
    , m_c(BUTTON_C, "C")
    , m_d(BUTTON_D, "D")
    , m_info(BUTTON_INFO, "Info")
    , m_fine(BUTTON_FINE, "Fine")
    , m_shift(BUTTON_SHIFT, "Shift")
    , m_default(BUTTON_DEFAULT, "Default")
    , m_inc(BUTTON_INC, "Inc")
    , m_dec(BUTTON_DEC, "Dec")
    , m_enter(BUTTON_ENTER, "Enter")
    , m_undo(BUTTON_UNDO, "Undo")
    , m_edit(BUTTON_EDIT, "Edit")
    , m_redo(BUTTON_REDO, "Redo")
    , m_minus(BUTTON_MINUS, "Minus")
    , m_plus(BUTTON_PLUS, "Plus")
    , m_function(BUTTON_FUNCTION, "Func")
    , m_mode(BUTTON_MODE, "Mode")
{
  m_buttons.add(m_exportBoled);
  m_buttons.add(m_exportSoled);

  m_buttons.add(m_setup);
  m_buttons.add(m_sound);
  m_buttons.add(m_preset);
  m_buttons.add(m_store);
  m_buttons.add(m_a);
  m_buttons.add(m_b);
  m_buttons.add(m_c);
  m_buttons.add(m_d);

  m_buttons.add(m_info);
  m_buttons.add(m_fine);
  m_buttons.add(m_shift);
  m_buttons.add(m_default);
  m_buttons.add(m_inc);
  m_buttons.add(m_rotary);
  m_buttons.add(m_dec);
  m_buttons.add(m_enter);
  m_buttons.add(m_undo);
  m_buttons.add(m_edit);
  m_buttons.add(m_redo);
  m_buttons.add(m_minus);
  m_buttons.add(m_plus);
  m_buttons.add(m_function);
  m_buttons.add(m_mode);

  m_buttons.set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);
  m_box.pack_start(m_boled, true, true);
  m_box.pack_start(m_buttons, false, false);
  add(m_box);

  m_rotary.set_range(-100, 100);
  m_rotary.set_value(0);
  m_rotary.signal_change_value().connect(sigc::mem_fun(this, &PlayPanel::onRotary));
}

PlayPanel::~PlayPanel()
{
}

void PlayPanel::setFrameBuffer(const nltools::msg::SetOLEDMessage &msg)
{
  m_boled.setBuffer(msg);
}

bool PlayPanel::onRotary(Gtk::ScrollType s, double v)
{
  nltools::Log::debug(__PRETTY_FUNCTION__, (int) s);
  auto b = Application::get().getBridges()->getBridge<FromEncoderBridge>();
  b->sendRotary(v);
  m_rotary.set_value(0);
  return false;
}
