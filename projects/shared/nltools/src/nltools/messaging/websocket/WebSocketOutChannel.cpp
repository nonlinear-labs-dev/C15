#include "WebSocketOutChannel.h"
#include <nltools/StringTools.h>
#include <nltools/logging/Log.h>
#include <netinet/tcp.h>
#include <glibmm.h>
#include <nltools/messaging/Message.h>
#include <pthread.h>

namespace nltools
{
  namespace msg
  {
    namespace ws
    {
      WebSocketOutChannel::WebSocketOutChannel(const std::string &targetMachine, guint port,
                                               nltools::threading::Priority p)
          : m_uri(nltools::string::concat("http://", targetMachine, ":", port))
          , m_soupSession(soup_session_new(), g_object_unref)
          , m_message(nullptr, g_object_unref)
          , m_connection(nullptr, g_object_unref)
          , m_mainThreadContextQueue(
                std::make_unique<threading::ContextBoundMessageQueue>(Glib::MainContext::get_default()))
          , m_contextThread([=] { this->backgroundThread(p); })
      {
      }

      WebSocketOutChannel::~WebSocketOutChannel()
      {
        while(!m_bgRunning)
        {
          using namespace std::chrono_literals;
          std::this_thread::sleep_for(10ms);
        }

        if(m_messageLoop && m_messageLoop->is_running())
          m_messageLoop->quit();

        if(m_contextThread.joinable())
          m_contextThread.join();
      }

      bool WebSocketOutChannel::send(const SerializedMessage &msg)
      {
        if(!m_connection || m_flushing)
          return false;

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
        return true;
      }

      void WebSocketOutChannel::flush(std::chrono::milliseconds timeout)
      {
        if(!m_connection)
          return;

        m_flushing = true;

        auto start = std::chrono::steady_clock::now();

        while(m_backgroundContextQueue->isPending())
        {
          auto now = std::chrono::steady_clock::now();
          auto elapsed = now - start;
          if(elapsed > timeout)
            return;
        }

        m_flushing = false;
      }

      bool WebSocketOutChannel::waitForConnection(std::chrono::milliseconds timeOut)
      {
        return m_connectionEstablishedWaiter.waitFor(timeOut);
      }

      void WebSocketOutChannel::onConnectionEstablished(std::function<void()> cb)
      {
        m_onConnectionEstablished = cb;
      }

      bool WebSocketOutChannel::isConnected() const
      {
        return m_connectionEstablishedWaiter.isNotified();
      }

      void WebSocketOutChannel::backgroundThread(nltools::threading::Priority p)
      {
        pthread_setname_np(pthread_self(), "WebSockOut");
        threading::setThisThreadPrio(p);

        auto mainContext = Glib::MainContext::get_default();

        auto m = Glib::MainContext::create();
        g_main_context_push_thread_default(m->gobj());

        m_backgroundContextQueue = std::make_unique<threading::ContextBoundMessageQueue>(m);
        m_messageLoop = Glib::MainLoop::create(m);
        m_backgroundContextQueue->pushMessage(std::bind(&WebSocketOutChannel::connect, this));

        auto c = mainContext->signal_timeout().connect_seconds(sigc::mem_fun(this, &WebSocketOutChannel::ping), 2);
        m_messageLoop->run();
        c.disconnect();
      }

      bool WebSocketOutChannel::ping()
      {
        send(detail::serialize(PingMessage()));
        return true;
      }

      void WebSocketOutChannel::connect()
      {
        m_bgRunning.store(true);
        m_message.reset(soup_message_new("GET", m_uri.c_str()));
        auto cb = reinterpret_cast<GAsyncReadyCallback>(&WebSocketOutChannel::onWebSocketConnected);
        soup_session_websocket_connect_async(m_soupSession.get(), m_message.get(), nullptr, nullptr, nullptr, cb, this);
      }

      void WebSocketOutChannel::onWebSocketConnected(SoupSession *session, GAsyncResult *res,
                                                     WebSocketOutChannel *pThis)
      {
        GError *error = nullptr;

        if(SoupWebsocketConnection *connection = soup_session_websocket_connect_finish(session, res, &error))
          pThis->connectWebSocket(connection);

        if(error)
        {
          nltools::Log::debug(pThis->m_uri, " -> ", error->message);
          g_error_free(error);
          pThis->reconnect();
        }
        else
        {
          pThis->signalConnectionEstablished();
        }
      }

      void WebSocketOutChannel::signalConnectionEstablished()
      {
        m_connectionEstablishedWaiter.notify();

        if(m_onConnectionEstablished)
          m_mainThreadContextQueue->pushMessage([this] { this->m_onConnectionEstablished(); });
      }

      void WebSocketOutChannel::reconnect()
      {
        auto sigTimeOut = this->m_messageLoop->get_context()->signal_timeout();
        sigTimeOut.connect_seconds_once(std::bind(&WebSocketOutChannel::connect, this), 2);
      }

      void WebSocketOutChannel::connectWebSocket(SoupWebsocketConnection *connection)
      {
        auto stream = soup_websocket_connection_get_io_stream(connection);
        auto outStream = g_io_stream_get_output_stream(stream);

        GError *error = nullptr;
        GSocket *socket = nullptr;
        g_object_get(outStream, "socket", &socket, nullptr);

        auto ret = g_socket_set_option(socket, SOL_TCP, TCP_NODELAY, 1, &error);

        if(error)
        {
          nltools::Log::error(error->message);
          g_error_free(error);
        }

        if(!ret)
          nltools::Log::warning("setting socket option NODELAY failed");

        m_connection.reset(connection);
      }
    }
  }
}
