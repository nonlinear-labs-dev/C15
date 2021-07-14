#pragma once
#include <gtkmm/hvbox.h>
#include <memory>
#include <array>
#include "Key.h"

class Keyboard : public Gtk::ActionBar
{
 public:
  Keyboard();

 private:
  std::array<std::unique_ptr<Key>, 60> m_keys {};
};
