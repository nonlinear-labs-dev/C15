#pragma once

#include <nltools/threading/ContextBoundMessageQueue.h>
#include <functional>
#include <nlohmann/json.hpp>
#include <libsoup/soup.h>
#include <glibmm.h>
#include <memory>
#include <future>

namespace nltools
{
  namespace msg
  {
    class WebSocketJsonAPI
    {
     public:
      using ReceiveCB = std::function<nlohmann::json(const nlohmann::json &)>;

      WebSocketJsonAPI(guint port, ReceiveCB cb);
      ~WebSocketJsonAPI();

      void sendAll(const nlohmann::json &msg);

     private:
      using tWebSocketPtr = std::unique_ptr<SoupWebsocketConnection, decltype(*g_object_unref)>;

      void backgroundThread();
      bool send(SoupWebsocketConnection *c, const nlohmann::json &msg);
      static void webSocket(SoupServer *, SoupWebsocketConnection *c, const char *, SoupClientContext *,
                            WebSocketJsonAPI *pThis);
      static void receiveMessage(SoupWebsocketConnection *c, gint, GBytes *message, WebSocketJsonAPI *pThis);

      guint m_port;
      ReceiveCB m_cb;
      std::unique_ptr<SoupServer, decltype(*g_object_unref)> m_server;
      Glib::RefPtr<Glib::MainLoop> m_messageLoop;
      std::unique_ptr<threading::ContextBoundMessageQueue> m_mainContextQueue;
      std::unique_ptr<threading::ContextBoundMessageQueue> m_bgContextQueue;
      std::list<tWebSocketPtr> m_connections;
      bool m_quit = false;
      std::mutex m_mutex;
      std::future<void> m_contextThread;
    };
  }
}