#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include <nltools/messaging/Message.h>

class Boled : public Gtk::DrawingArea
{
 public:
  Boled();
  virtual ~Boled();

  void setBuffer(const nltools::msg::SetOLEDMessage& msg);

  bool on_draw(const ::Cairo::RefPtr<::Cairo::Context>& cr) override;

 private:
  nltools::msg::SetOLEDMessage m_frameBuffer;
};

#endif
