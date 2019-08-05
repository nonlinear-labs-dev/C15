#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include <functional>

class Button : public Gtk::Button
{
 public:
  Button(int id, const std::string& title = "");
  Button(const std::string& title, std::function<void()> cb);
  virtual ~Button();

  void setLed(int idx, bool state);

 protected:
  bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr) override;
  void on_pressed() override;
  void on_released() override;

 private:
  int m_buttonId;
  bool m_state = false;
  std::function<void()> m_cb;
};

#endif
