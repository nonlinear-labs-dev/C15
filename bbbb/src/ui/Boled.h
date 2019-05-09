#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include <io/network/WebSocketServer.h>

class Boled : public Gtk::DrawingArea
{
 public:
  Boled();
  virtual ~Boled();

  void setBuffer(WebSocketServer::tMessage frameBuffer);

  bool on_draw(const ::Cairo::RefPtr<::Cairo::Context>& cr) override;

 private:
  WebSocketServer::tMessage m_frameBuffer;
};

#endif
