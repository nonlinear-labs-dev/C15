#pragma once

#include <nltools/messaging/OutChannel.h>
#include <nltools/threading/ContextBoundMessageQueue.h>
#include <nltools/threading/BackgroundThreadWaiter.h>
#include <nltools/threading/Threading.h>

#include <memory>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <libsoup/soup.h>

namespace nltools
{
  namespace msg
  {
    namespace ws
    {
      class WebSocketOutChannel : public OutChannel
      {
       public:
        WebSocketOutChannel(const std::string &targetMachine, guint port, nltools::threading::Priority p,
                            std::function<void()> connectionEstablishedCB);

        ~WebSocketOutChannel() override;

        bool send(const SerializedMessage &msg) override;
        void flush(const std::chrono::milliseconds timeout) override;
        bool waitForConnection(std::chrono::milliseconds timeOut) override;
        bool isConnected() const override;
        void signalConnectionEstablished();

       private:
        void connect();
        void connectWebSocket(SoupWebsocketConnection *connection);

        static void onWebSocketConnected(SoupSession *session, GAsyncResult *res, WebSocketOutChannel *pThis);

        void reconnect();
        void backgroundThread(nltools::threading::Priority p);
        bool ping();

        using tSessionPtr = std::unique_ptr<SoupSession, decltype(*g_object_unref)>;
        using tWebSocketPtr = std::unique_ptr<SoupWebsocketConnection, decltype(*g_object_unref)>;
        using tMessagePtr = std::unique_ptr<SoupMessage, decltype(*g_object_unref)>;

        std::string m_uri;
        tSessionPtr m_soupSession;
        tMessagePtr m_message;
        tWebSocketPtr m_connection;

        std::unique_ptr<threading::ContextBoundMessageQueue> m_backgroundContextQueue;
        std::unique_ptr<threading::ContextBoundMessageQueue> m_mainThreadContextQueue;
        Glib::RefPtr<Glib::MainLoop> m_messageLoop;

        std::atomic_bool m_bgRunning = { false };
        BackgroundThreadWaiter m_connectionEstablishedWaiter;
        std::function<void()> m_onConnectionEstablished;
        std::thread m_contextThread;
        bool m_flushing = false;
      };
    }
  }
}
