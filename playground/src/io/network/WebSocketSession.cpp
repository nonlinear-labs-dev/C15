#include <io/network/WebSocketSession.h>
#include <device-settings/DebugLevel.h>
#include <string.h>
#include <Application.h>
#include <Options.h>
#include <netinet/tcp.h>

using namespace std::chrono_literals;

WebSocketSession::WebSocketSession()
    : m_soupSession(soup_session_new(), g_object_unref)
    , m_message(nullptr, g_object_unref)
    , m_connection(nullptr, g_object_unref)
    , m_retry(std::bind(&WebSocketSession::connect, this))
{
  connect();
}

WebSocketSession::~WebSocketSession()
{
}

sigc::connection WebSocketSession::onMessageReceived(Domain d, const sigc::slot<void, tMessage> &cb)
{
  return m_onMessageReceived[d].connect(cb);
}

sigc::connection WebSocketSession::onConnectionEstablished(const sigc::slot<void> &cb)
{
  return m_onConnectionEstablished.connect(cb);
}

void WebSocketSession::connect()
{
  auto uri = "http://" + Application::get().getOptions()->getBBBB() + ":11111";
  m_message.reset(soup_message_new("GET", uri.c_str()));
  auto cb = (GAsyncReadyCallback) &WebSocketSession::onWebSocketConnected;
  soup_session_websocket_connect_async(m_soupSession.get(), m_message.get(), nullptr, nullptr, nullptr, cb, this);
}

void WebSocketSession::onWebSocketConnected(SoupSession *session, GAsyncResult *res, WebSocketSession *pThis)
{
  GError *error = nullptr;

  if(SoupWebsocketConnection *connection = soup_session_websocket_connect_finish(session, res, &error))
  {
    pThis->connectWebSocket(connection);
  }

  if(error)
  {
    DebugLevel::warning(error->message);
    g_error_free(error);
    pThis->reconnect();
  }
}

void WebSocketSession::reconnect()
{
  if(!m_retry.isPending())
    m_retry.refresh(2s);
}

void WebSocketSession::connectWebSocket(SoupWebsocketConnection *connection)
{
  g_signal_connect(connection, "message", G_CALLBACK(&WebSocketSession::receiveMessage), this);
  g_object_set(connection, "keepalive-interval", 5, nullptr);
  g_object_ref(connection);

  auto stream = soup_websocket_connection_get_io_stream(connection);
  auto outStream = g_io_stream_get_output_stream(stream);

  GError *error = nullptr;
  GSocket *socket = nullptr;
  g_object_get(outStream, "socket", &socket, nullptr);

  auto ret = g_socket_set_option(socket, SOL_TCP, TCP_NODELAY, 1, &error);

  if(error)
  {
    DebugLevel::warning(error->message);
    g_error_free(error);
  }

  if(!ret)
  {
    DebugLevel::warning("setting socket option NODELAY failed");
  }

  m_connection.reset(connection);
  m_onConnectionEstablished();
}

void WebSocketSession::sendMessage(Domain d, tMessage msg)
{
  gsize len = 0;
  if(auto data = reinterpret_cast<const int8_t *>(msg->get_data(len)))
  {
    auto cp = new int8_t[len + 1];
    cp[0] = (int8_t) d;
    std::copy(data, data + len, cp + 1);
    sendMessage(Glib::Bytes::create(cp, len + 1));
  }
}

void WebSocketSession::sendMessage(tMessage msg)
{
  if(m_connection)
  {
    auto state = soup_websocket_connection_get_state(m_connection.get());

    if(state == SOUP_WEBSOCKET_STATE_OPEN)
    {
      gsize len = 0;
      auto data = msg->get_data(len);
      soup_websocket_connection_send_binary(m_connection.get(), data, len);
    }
    else
    {
      m_connection.reset();
      reconnect();
    }
  }
}

void WebSocketSession::receiveMessage(SoupWebsocketConnection *self, gint type, GBytes *message,
                                      WebSocketSession *pThis)
{
  tMessage msg = Glib::wrap(message);
  gsize len = 0;
  auto data = reinterpret_cast<const uint8_t *>(msg->get_data(len));
  Domain d = (Domain)(data[0]);

  auto dup = g_memdup(data + 1, len - 1);
  pThis->m_onMessageReceived[d](Glib::Bytes::create(dup, len - 1));
}
