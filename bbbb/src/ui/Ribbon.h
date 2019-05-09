#pragma once

#include <gtkmm-3.0/gtkmm.h>

class Ribbon : public Gtk::HScale
{
 public:
  Ribbon();
  virtual bool isUpperRibbon();

 protected:
  void on_value_changed() override;
  bool m_upperRibbon;
};
