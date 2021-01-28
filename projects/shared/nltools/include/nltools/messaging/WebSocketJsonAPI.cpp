#include "WebSocketJsonAPI.h"
#include <nltools/logging/Log.h>

namespace nltools
{
  namespace msg
  {

    WebSocketJsonAPI::WebSocketJsonAPI(guint port, WebSocketJsonAPI::ReceiveCB cb)
        : m_port(port)
        , m_cb(cb)
        , m_server(soup_server_new(nullptr, nullptr), g_object_unref)
        , m_mainContextQueue(std::make_unique<threading::ContextBoundMessageQueue>(Glib::MainContext::get_default()))
        , m_messageLoop(Glib::MainLoop::create(Glib::MainContext::create()))
        , m_bgContextQueue(std::make_unique<threading::ContextBoundMessageQueue>(m_messageLoop->get_context()))
        , m_contextThread(std::async(std::launch::async, [=] { this->backgroundThread(); }))
    {
    }

    WebSocketJsonAPI::~WebSocketJsonAPI()
    {
      std::unique_lock<std::mutex> l(m_mutex);
      m_quit = true;
      m_messageLoop->quit();
      m_contextThread.wait();
    }

    void WebSocketJsonAPI::sendAll(const nlohmann::json &msg)
    {
      m_bgContextQueue->pushMessage(
          [this, msg]() { m_connections.remove_if([this, msg](auto &c) { return !send(c.get(), msg); }); });
    }

    void WebSocketJsonAPI::backgroundThread()
    {
      std::unique_lock<std::mutex> l(m_mutex);

      g_main_context_push_thread_default(m_messageLoop->get_context()->gobj());

      GError *error = nullptr;

      auto callback = (SoupServerWebsocketCallback) &WebSocketJsonAPI::webSocket;
      soup_server_add_websocket_handler(m_server.get(), nullptr, nullptr, nullptr, callback, this, nullptr);
      soup_server_listen_all(m_server.get(), m_port, SOUP_SERVER_LISTEN_IPV4_ONLY, &error);

      if(error)
      {
        Log::error(error->message);
        g_error_free(error);
      }

      if(!m_quit)
      {
        l.unlock();
        m_messageLoop->run();
      }
    }

    bool WebSocketJsonAPI::send(SoupWebsocketConnection *c, const nlohmann::json &msg)
    {
      if(std::find_if(m_connections.begin(), m_connections.end(), [&](auto &a) { return c == a.get(); })
         == m_connections.end())
        return false;

      auto state = soup_websocket_connection_get_state(c);

      if(state != SOUP_WEBSOCKET_STATE_OPEN)
        return false;

      auto flattened = msg.dump();
      soup_websocket_connection_send_binary(c, flattened.c_str(), flattened.size());
      return true;
    }

    void WebSocketJsonAPI::webSocket(SoupServer *, SoupWebsocketConnection *c, const char *, SoupClientContext *,
                                     WebSocketJsonAPI *pThis)
    {
      g_signal_connect(c, "message", G_CALLBACK(&WebSocketJsonAPI::receiveMessage), pThis);
      g_object_ref(c);
      pThis->m_connections.push_back(tWebSocketPtr(c, g_object_unref));
    }

    void WebSocketJsonAPI::receiveMessage(SoupWebsocketConnection *c, gint, GBytes *message, WebSocketJsonAPI *pThis)
    {
      gsize length = 0;
      auto ptr = static_cast<const char *>(g_bytes_get_data(message, &length));
      try
      {
        pThis->m_mainContextQueue->pushMessage([pThis, c, j = nlohmann::json::parse(ptr, ptr + length)] {
          try
          {
            pThis->send(c, pThis->m_cb(j));
          }
          catch(...)
          {
            nltools::Log::warning("Could not handle incoming web-socket message");
          }
        });
      }
      catch(...)
      {
        nltools::Log::warning("Could not parse incoming web-socket message");
      }
    }

  }
}