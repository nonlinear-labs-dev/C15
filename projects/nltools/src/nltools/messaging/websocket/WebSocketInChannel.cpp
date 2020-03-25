#include "WebSocketInChannel.h"
#include <nltools/messaging/Message.h>
#include <nltools/threading/ContextBoundMessageQueue.h>
#include <nltools/logging/Log.h>
#include <nltools/StringTools.h>
#include <netinet/tcp.h>
#include <glibmm.h>
#include <utility>

namespace nltools
{
  namespace msg
  {
    namespace ws
    {
      WebSocketInChannel::WebSocketInChannel(Callback cb, guint port)
          : InChannel(std::move(cb))
          , m_port(port)
          , m_server(soup_server_new(nullptr, nullptr), g_object_unref)
          , m_mainContextQueue(std::make_unique<threading::ContextBoundMessageQueue>(Glib::MainContext::get_default()))
          , m_contextThread(std::bind(&WebSocketInChannel::backgroundThread, this))
      {
        m_conditionEstablishedThreadWaiter.wait();
      }

      WebSocketInChannel::~WebSocketInChannel()
      {
        if(m_messageLoop)
          m_messageLoop->quit();

        if(m_contextThread.joinable())
          m_contextThread.join();
      }

      void WebSocketInChannel::backgroundThread()
      {
        pthread_setname_np(pthread_self(), "WebSockIn");

        auto m = Glib::MainContext::create();
        g_main_context_push_thread_default(m->gobj());

        GError *error = nullptr;

        auto callback = (SoupServerWebsocketCallback) &WebSocketInChannel::webSocket;
        soup_server_add_websocket_handler(m_server.get(), nullptr, nullptr, nullptr, callback, this, nullptr);
        soup_server_listen_all(m_server.get(), m_port, SOUP_SERVER_LISTEN_IPV4_ONLY, &error);

        if(error)
        {
          if(error->domain == G_IO_ERROR && error->code == G_IO_ERROR_ADDRESS_IN_USE)
          {
            nltools::Log::error("Could not listen to port ", m_port, ", trying to output the current listener:");
            auto script
                = string::concat("sh -c \"lsof -i:", m_port, " | grep LISTEN | cut -f 1 -d ' ' | tr '\n' ' '\"");
            gchar *output = nullptr;
            g_spawn_command_line_sync(script.c_str(), &output, nullptr, nullptr, nullptr);

            if(output)
              nltools::throwException("Could not listen to port ", m_port, ", because '", output, "' already owns it.");

            nltools::throwException("Could not listen to port ", m_port);
          }

          nltools::Log::error(error->message);
          g_error_free(error);
        }

        m_messageLoop = Glib::MainLoop::create(m);
        m_conditionEstablishedThreadWaiter.notify();
        m_messageLoop->run();
      }

      void WebSocketInChannel::webSocket(SoupServer *, SoupWebsocketConnection *c, const char *, SoupClientContext *,
                                         WebSocketInChannel *pThis)
      {
        g_signal_connect(c, "message", G_CALLBACK(&WebSocketInChannel::receiveMessage), pThis);
        g_object_ref(c);
#ifdef SOUP_VERSION_2_58
        g_object_set(c, "keepalive-interval", 5, nullptr);
#endif
        pThis->m_connections.push_back(tWebSocketPtr(c, g_object_unref));
      }

      void WebSocketInChannel::receiveMessage(SoupWebsocketConnection *, gint, GBytes *message,
                                              WebSocketInChannel *pThis)
      {
        auto bytes = Glib::wrap(g_bytes_ref(message));
        pThis->m_mainContextQueue->pushMessage([=] { pThis->onMessageReceived(bytes); });
      }
    }
  }
}
