#include "WebSocketRequest.h"
#include "WebSocketOutStream.h"

WebSocketRequest::WebSocketRequest(SoupWebsocketConnection *socket, GBytes *data)
    : m_socket(socket)
{
  g_object_ref(m_socket);

  gsize len = 0;
  Glib::ustring uri = (const char *) g_bytes_get_data(data, &len);
  auto q = uri.find('?');

  if(q != Glib::ustring::npos)
  {
    m_path = uri.substr(0, q);
    createMapFromQueryString(uri.substr(q + 1));
  }
  else
  {
    m_path = uri;
  }
}

WebSocketRequest::~WebSocketRequest()
{
  g_object_unref(m_socket);
}

const SoupWebsocketConnection *WebSocketRequest::getSocket() const
{
  return m_socket;
}

std::shared_ptr<OutStream> WebSocketRequest::createStream(const Glib::ustring &contentType, bool zip)
{
  return std::shared_ptr<OutStream>(new WebSocketOutStream(m_socket));
}

void WebSocketRequest::okAndComplete()
{
}

void WebSocketRequest::unpause()
{
}

void WebSocketRequest::pause()
{
}

Glib::ustring WebSocketRequest::getPath()
{
  return m_path;
}
