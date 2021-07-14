#pragma once

#include <xml/OutStream.h>
#include <libsoup/soup.h>
#include <sstream>

class WebSocketOutStream : public OutStream
{
 private:
  typedef OutStream super;

 public:
  explicit WebSocketOutStream(SoupWebsocketConnection *connection);
  ~WebSocketOutStream() override;

 private:
  void implWrite(const Glib::ustring &str) override;
  void implWrite(const void *buf, size_t numBytes) override;
  SoupWebsocketConnection *m_connection = nullptr;
  std::stringstream m_text;
};
