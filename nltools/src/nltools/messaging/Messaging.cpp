#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/websocket/WebSocketOutChannel.h>
#include <nltools/messaging/websocket/WebSocketInChannel.h>
#include <nltools/logging/Log.h>
#include <nltools/ExceptionTools.h>
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

      static std::vector<ChannelConfiguration> inChannelConfig;
      static std::vector<ChannelConfiguration> outChannelConfig;
      static OutChannels outChannels;
      static InChannels inChannels;
      static Signals signals;
      static ConnectionSignals connectionSignals;

      static void notifyClients(const SerializedMessage &s, EndPoint endPoint)
      {
        gsize numBytes = 0;
        auto data = reinterpret_cast<const uint16_t *>(s->get_data(numBytes));
        auto type = static_cast<MessageType>(data[0]);
        signals.at(std::make_pair(type, endPoint))(s);
      }

      static void createInChannels(const Configuration &conf)
      {
        inChannelConfig = conf.offerEndpoints;
        for(const auto &c : conf.offerEndpoints)
        {
          auto cb = [peer = c.peer](const auto &s) { notifyClients(s, peer); };

          parseURI(c.uri, [=](auto scheme, auto, auto, auto port) {
            nltools_assertOnDevPC(scheme == "ws");  // Currently, only web sockets are supported
            inChannels[c.peer] = std::make_unique<ws::WebSocketInChannel>(cb, port);
            signals[std::make_pair(MessageType::Ping, c.peer)];
          });
        }
      }

      static void createOutChannels(const Configuration &conf)
      {
        outChannelConfig = conf.useEndpoints;
        for(const auto &c : conf.useEndpoints)
        {
          parseURI(c.uri, [=](auto scheme, auto host, auto, auto port) {
            nltools_assertOnDevPC(scheme == "ws");  // Currently, only web sockets are supported
            outChannels[c.peer] = std::make_unique<ws::WebSocketOutChannel>(host, port);
            outChannels[c.peer]->onConnectionEstablished([peer = c.peer] { connectionSignals[peer](); });
          });
        }
      }

      static sigc::connection connectReceiver(MessageType type, EndPoint endPoint,
                                              std::function<void(const SerializedMessage &)> cb)
      {
        auto ret = signals[std::make_pair(type, endPoint)].connect(cb);
        return ret;
      }

      void send(nltools::msg::EndPoint receiver, const SerializedMessage &msg)
      {
        try {
          outChannels.at(receiver)->send(msg);
        } catch(...) {
          nltools::Log::error("no such receiver:", toString(receiver));
        }
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
        , uri(nltools::string::concat("ws://", "localhost", ":", getPortFor(p)))
    {
    }

    ChannelConfiguration::ChannelConfiguration(EndPoint p, const std::string &hostName)
        : peer(p)
        , uri(nltools::string::concat("ws://", hostName, ":", getPortFor(p)))
    {
    }

    void init(const Configuration &conf)
    {
      deInit();
      detail::createInChannels(conf);
      detail::createOutChannels(conf);
    }

    Configuration swapConfig(const Configuration& conf)
    {
      Configuration cfg;
      cfg.offerEndpoints = detail::inChannelConfig;
      cfg.useEndpoints = detail::outChannelConfig;
      init(conf);
      return cfg;
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
