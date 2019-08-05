#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include <nltools/messaging/Message.h>

class Boled : public Gtk::DrawingArea
{
 public:
  Boled();
  ~Boled() override;

  void setBuffer(const nltools::msg::SetOLEDMessage& msg);

  bool on_draw(const ::Cairo::RefPtr<::Cairo::Context>& cr) override;
  void exportBoled() const;
  void exportSoled() const;

 private:
  void exportOled(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const std::string& fileName) const;
  std::string createPNGFileName(const std::string& prefix) const;

  nltools::msg::SetOLEDMessage m_frameBuffer;
};

#endif
