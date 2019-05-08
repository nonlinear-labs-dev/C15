#include "WebSocketOutStream.h"

WebSocketOutStream::WebSocketOutStream(SoupWebsocketConnection *connection)
    : m_connection(connection)
{
}

WebSocketOutStream::~WebSocketOutStream()
{
  soup_websocket_connection_send_text(m_connection, m_text.str().c_str());
}

void WebSocketOutStream::implWrite(const Glib::ustring &str)
{
  m_text << str.raw();
}

void WebSocketOutStream::implWrite(const void *buf, size_t numBytes)
{
  m_text.write((const char *) buf, numBytes);
}
