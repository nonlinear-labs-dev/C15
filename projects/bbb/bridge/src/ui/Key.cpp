#include <nltools/messaging/Message.h>
#include "Key.h"

inline bool isBlackKey(int keyPos)
{
  keyPos = keyPos % 12;
  return keyPos == 1 || keyPos == 3 || keyPos == 6 || keyPos == 8 || keyPos == 10;
}

Key::Key(int keyPos)
    : m_keyPos { keyPos }
    , m_cssProvider { Gtk::CssProvider::create() }
{
  if(isBlackKey(m_keyPos))
    get_style_context()->add_class("key-black");
  else
    get_style_context()->add_class("key-white");

  get_style_context()->add_provider(m_cssProvider, -1);
  m_cssProvider->load_from_data(".key-white {background-image: none; background-color: white; }\
                                      .key-black {background-image: none; background-color: black; }");

  set_tooltip_text(std::to_string(keyPos));
  set_vexpand(true);
}

void Key::on_pressed()
{
  nltools::msg::Keyboard::NoteDown msg { m_keyPos };
  nltools::msg::send(nltools::msg::EndPoint::AudioEngine, msg);
}

void Key::on_released()
{
  nltools::msg::Keyboard::NoteUp msg { m_keyPos };
  nltools::msg::send(nltools::msg::EndPoint::AudioEngine, msg);
}
