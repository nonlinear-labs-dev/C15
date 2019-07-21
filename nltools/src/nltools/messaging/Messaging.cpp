#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/websocket/WebSocketOutChannel.h>
#include <nltools/messaging/websocket/WebSocketInChannel.h>
#include <nltools/logging/Log.h>
#include <nltools/StringTools.h>
#include <memory>
#include <map>
#include <list>

namespace nltools
{
  namespace msg
  {
    namespace detail
    {
      using OutChannels = std::map<EndPoint, std::unique_ptr<OutChannel>>;
      using InChannels = std::map<EndPoint, std::unique_ptr<InChannel>>;
      using Signals = std::map<std::pair<MessageType, EndPoint>, sigc::signal<void, const SerializedMessage &>>;
      using ConnectionSignals = std::map<EndPoint, sigc::signal<void>>;

      static OutChannels outChannels;
      static InChannels inChannels;
      static Signals signals;
      static ConnectionSignals connectionSignals;

      static void notifyClients(const SerializedMessage &s, EndPoint endPoint)
      {
        gsize numBytes = 0;
        auto data = reinterpret_cast<const uint16_t *>(s->get_data(numBytes));
        auto type = static_cast<MessageType>(data[0]);
        signals[std::make_pair(type, endPoint)](s);
      }

      static void createInChannels(const Configuration &conf, std::mutex &libSoupMutex)
      {
        for(const auto &c : conf.offerEndpoints)
        {
          auto cb = [peer = c.peer](const auto &s) { notifyClients(s, peer); };

          parseURI(c.uri, [=, &libSoupMutex](auto scheme, auto, auto, auto port) {
            assert(scheme == "ws");  // Currently, only web sockets are supported
            std::unique_lock<std::mutex> lock(libSoupMutex);
            inChannels[c.peer] = std::make_unique<ws::WebSocketInChannel>(cb, port, libSoupMutex);
          });
        }
      }

      static void createOutChannels(const Configuration &conf, std::mutex &libSoupMutex)
      {
        for(const auto &c : conf.useEndpoints)
        {
          parseURI(c.uri, [=, &libSoupMutex](auto scheme, auto host, auto, auto port) {
            assert(scheme == "ws");  // Currently, only web sockets are supported
            std::unique_lock<std::mutex> lock(libSoupMutex);
            outChannels[c.peer] = std::make_unique<ws::WebSocketOutChannel>(host, port, libSoupMutex);
            outChannels[c.peer]->onConnectionEstablished([peer = c.peer] { connectionSignals[peer](); });
          });
        }
      }

      static sigc::connection connectReceiver(MessageType type, EndPoint endPoint,
                                              std::function<void(const SerializedMessage &)> cb)
      {
        return signals[std::make_pair(type, endPoint)].connect(cb);
      }

      void send(nltools::msg::EndPoint receiver, SerializedMessage msg)
      {
        outChannels.at(receiver)->send(msg);
      }

      sigc::connection receiveSerialized(MessageType type, EndPoint receivingEndPoint,
                                         std::function<void(const SerializedMessage &)> cb)
      {
        return connectReceiver(type, receivingEndPoint, cb);
      }
    }

    bool waitForConnection(EndPoint receiver, std::chrono::milliseconds timeOut)
    {
      return detail::outChannels.at(receiver)->waitForConnection(timeOut);
    }

    ChannelConfiguration::ChannelConfiguration(EndPoint p)
        : peer(p)
        , uri(concat("ws://", "localhost", ":", getPortFor(p)))
    {
    }

    ChannelConfiguration::ChannelConfiguration(EndPoint p, const std::string &hostName)
        : peer(p)
        , uri(concat("ws://", hostName, ":", getPortFor(p)))
    {
    }

    void init(const Configuration &conf)
    {
      static std::mutex libSoupMutex;
      deInit();
      detail::createInChannels(conf, libSoupMutex);
      detail::createOutChannels(conf, libSoupMutex);
    }

    void deInit()
    {
      detail::outChannels.clear();
      detail::inChannels.clear();
      detail::signals.clear();
      detail::connectionSignals.clear();
    }

    uint getPortFor(EndPoint p)
    {
      return static_cast<uint>(p) + 40100;
    }

    sigc::connection onConnectionEstablished(EndPoint endPoint, std::function<void()> cb)
    {
      auto ret = detail::connectionSignals[endPoint].connect(cb);

      if(detail::outChannels.at(endPoint)->isConnected())
        cb();

      return ret;
    }
  }
}
