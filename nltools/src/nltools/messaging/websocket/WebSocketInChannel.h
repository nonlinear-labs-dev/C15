#pragma once

#include <nltools/messaging/InChannel.h>
#include <nltools/threading/ContextBoundMessageQueue.h>
#include <nltools/threading/BackgroundThreadWaiter.h>
#include <libsoup/soup.h>
#include <memory>
#include <thread>

namespace nltools
{
  namespace msg
  {
    namespace ws
    {
      class WebSocketInChannel : public InChannel
      {
       public:
        WebSocketInChannel(Callback cb, guint port);
        ~WebSocketInChannel() override;

       private:
        void backgroundThread();

        static void webSocket(SoupServer *server, SoupWebsocketConnection *connection, const char *pathStr,
                              SoupClientContext *client, WebSocketInChannel *pThis);
        static void receiveMessage(SoupWebsocketConnection *, gint, GBytes *message, WebSocketInChannel *pThis);

        using tWebSocketPtr = std::unique_ptr<SoupWebsocketConnection, decltype(*g_object_unref)>;

        guint m_port;
        std::unique_ptr<SoupServer, decltype(*g_object_unref)> m_server;
        Glib::RefPtr<Glib::MainLoop> m_messageLoop;
        std::unique_ptr<threading::ContextBoundMessageQueue> m_mainContextQueue;
        std::list<tWebSocketPtr> m_connections;

        BackgroundThreadWaiter m_conditionEstablishedThreadWaiter;
        std::thread m_contextThread;
      };
    }
  }
}
