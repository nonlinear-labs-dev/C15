#pragma once

#include <nltools/messaging/OutChannel.h>
#include <nltools/threading/ContextBoundMessageQueue.h>

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
        WebSocketOutChannel(const std::string &targetMachine, guint port, std::mutex &);
        ~WebSocketOutChannel() override;

        void send(const SerializedMessage &msg) override;
        bool waitForConnection(std::chrono::milliseconds timeOut) override;
        void onConnectionEstablished(std::function<void()> cb) override;
        bool isConnected() const override;

        void signalConnectionEstablished();

       private:
        void connect();
        void connectWebSocket(SoupWebsocketConnection *connection);
        static void onWebSocketConnected(SoupSession *session, GAsyncResult *res, WebSocketOutChannel *pThis);
        void reconnect();
        void backgroundThread();
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
        std::thread m_contextThread;

        std::atomic<bool> m_connectionEstablished = false;
        std::mutex m_conditionMutex;
        std::condition_variable m_connectionEstablishedCondition;
        std::function<void()> m_onConnectionEstablished;
      };
    }
  }
}
