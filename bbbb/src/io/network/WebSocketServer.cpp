#include <io/network/WebSocketServer.h>
#include <netinet/tcp.h>


WebSocketServer::WebSocketServer() :
    m_server(soup_server_new(NULL), g_object_unref)
{
  GError *error = nullptr;

  auto callback = (SoupServerWebsocketCallback) &WebSocketServer::webSocket;
  soup_server_add_websocket_handler(m_server.get(), nullptr, nullptr, nullptr, callback, this, nullptr);
  soup_server_listen_all(m_server.get(), 11111, SOUP_SERVER_LISTEN_IPV4_ONLY, &error);

  if(error)
  {
    TRACE(error->message);
    g_error_free(error);
  }
}

WebSocketServer::~WebSocketServer()
{
}

sigc::connection WebSocketServer::onMessageReceived(Domain d, const sigc::slot<void, tMessage> &cb)
{
  return m_onMessageReceived[d].connect(cb);
}


void WebSocketServer::webSocket(SoupServer *, SoupWebsocketConnection *c, const char *, SoupClientContext *, WebSocketServer *pThis)
{
  pThis->connectWebSocket(c);
}

void WebSocketServer::connectWebSocket(SoupWebsocketConnection *connection)
{
  g_signal_connect(connection, "message", G_CALLBACK (&WebSocketServer::receiveMessage), this);
  g_object_ref(connection);
  g_object_set(connection, "keepalive-interval", 5, nullptr);

  auto stream = soup_websocket_connection_get_io_stream(connection);
  auto outStream = g_io_stream_get_output_stream(stream);

  GError *error = nullptr;
  GSocket *socket = nullptr;
  g_object_get(outStream, "socket", &socket, nullptr);

  auto ret = g_socket_set_option (socket, SOL_TCP, TCP_NODELAY, 1, &error);

  if(error)
  {
    TRACE(error->message);
    g_error_free(error);
  }

  if(!ret)
  {
    TRACE("setting socket option NODELAY failed");
  }

  m_connections.push_back(tWebSocketPtr(connection, g_object_unref));
}

void WebSocketServer::sendMessage(tMessage msg)
{
  m_connections.remove_if([&] (auto &c)
  {
    auto state = soup_websocket_connection_get_state (c.get());

    if (state == SOUP_WEBSOCKET_STATE_OPEN)
    {
      gsize len = 0;
      auto data = msg->get_data(len);
      soup_websocket_connection_send_binary(c.get(), data, len);
      return false;
    }

    TRACE("connection state: " << state);
    return true;
  });
}

void WebSocketServer::receiveMessage(SoupWebsocketConnection *self, gint type, GBytes *message, WebSocketServer *pThis)
{
  tMessage msg = Glib::wrap(message);

  gsize len = 0;
  auto data = reinterpret_cast<const uint8_t*>(msg->get_data(len));
  Domain d = (Domain)data[0];
  auto dup = g_memdup(data + 1, len - 1);
  pThis->m_onMessageReceived[d](Glib::Bytes::create(dup, len - 1));
}
