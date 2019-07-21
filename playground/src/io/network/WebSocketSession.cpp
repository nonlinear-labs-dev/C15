#include <io/network/WebSocketSession.h>
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <Options.h>
#include <netinet/tcp.h>

using namespace std::chrono_literals;

WebSocketSession::WebSocketSession()
    : m_soupSession(nullptr, g_object_unref)
    , m_message(nullptr, g_object_unref)
    , m_connection(nullptr, g_object_unref)
    , m_defaultContextQueue(std::make_unique<nltools::threading::ContextBoundMessageQueue>(Glib::MainContext::get_default()))
    , m_contextThread(std::bind(&WebSocketSession::backgroundThread, this))
{
}

WebSocketSession::~WebSocketSession()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  m_messageLoop->quit();
  m_contextThread.join();
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

void WebSocketSession::startListening()
{
  m_isListenening = true;
}

void WebSocketSession::backgroundThread()
{
  auto m = Glib::MainContext::create();
  g_main_context_push_thread_default(m->gobj());
  this->m_soupSession.reset(soup_session_new());
  this->m_backgroundContextQueue = std::make_unique<nltools::threading::ContextBoundMessageQueue>(m);
  this->m_messageLoop = Glib::MainLoop::create(m);
  m_backgroundContextQueue->pushMessage(std::bind(&WebSocketSession::connect, this));
  this->m_messageLoop->run();
}

sigc::connection WebSocketSession::onMessageReceived(Domain d, const sigc::slot<void, tMessage> &cb)
{
  return m_onMessageReceived[d].connect(cb);
}

sigc::connection WebSocketSession::onConnectionEstablished(const sigc::slot<void> &cb)
{
  return m_onConnectionEstablished.connect(cb);
}

#if _DEVELOPMENT_PC
void WebSocketSession::simulateReceivedDebugMessage(WebSocketSession::DebugScriptEntry &&e)
{
  debugScriptExpiration.setCallback([this] {
    if(!debugScriptEntries.empty())
    {
      const auto &s = debugScriptEntries.front();
      DebugLevel::warning("Executing simulated debug message.");
      m_onMessageReceived[s.domain](s.msg);
      debugScriptEntries.pop_front();

      if(!debugScriptEntries.empty())
      {
        const auto &s = debugScriptEntries.front();
        debugScriptExpiration.refresh(s.delay);
      }
    }
  });

  debugScriptEntries.push_back(std::move(e));

  if(!debugScriptExpiration.isPending())
    debugScriptExpiration.refresh(debugScriptEntries.front().delay);
}
#endif

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
    g_error_free(error);
    pThis->reconnect();
  }
}

void WebSocketSession::reconnect()
{
  if(!Application::get().isQuit())
  {
    auto sigTimeOut = this->m_messageLoop->get_context()->signal_timeout();
    sigTimeOut.connect_seconds_once(std::bind(&WebSocketSession::connect, this), 2);
  }
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

  m_defaultContextQueue->pushMessage([=]() { m_onConnectionEstablished(); });
}

void WebSocketSession::sendMessage(Domain d, tMessage msg)
{
  gsize len = 0;
  if(auto data = reinterpret_cast<const int8_t *>(msg->get_data(len)))
  {
    int8_t cp[len + 1];
    cp[0] = static_cast<int8_t>(d);
    std::copy(data, data + len, cp + 1);
    sendMessage(Glib::Bytes::create(cp, len + 1));
  }
}

void WebSocketSession::sendMessage(tMessage msg)
{
  m_backgroundContextQueue->pushMessage([=]() {
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
  });
}

void WebSocketSession::receiveMessage(SoupWebsocketConnection *self, gint type, GBytes *message,
                                      WebSocketSession *pThis)
{
  if(pThis->m_isListenening)
  {
    tMessage msg = Glib::wrap(message);
    gsize len = 0;
    auto data = reinterpret_cast<const uint8_t *>(msg->get_data(len));
    auto d = static_cast<Domain>(data[0]);
    auto byteMessage = Glib::Bytes::create(data + 1, len - 1);
    pThis->m_defaultContextQueue->pushMessage([=]() { pThis->m_onMessageReceived[d](byteMessage); });
  }
}
