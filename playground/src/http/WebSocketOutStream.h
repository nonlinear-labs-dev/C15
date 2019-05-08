#pragma once

#include <xml/OutStream.h>
#include <libsoup/soup.h>

class WebSocketOutStream : public OutStream
{
 private:
  typedef OutStream super;

 public:
  WebSocketOutStream(SoupWebsocketConnection *connection);
  virtual ~WebSocketOutStream();

 private:
  void implWrite(const Glib::ustring &str) override;
  void implWrite(const void *buf, size_t numBytes) override;
  SoupWebsocketConnection *m_connection = nullptr;
  stringstream m_text;
};
