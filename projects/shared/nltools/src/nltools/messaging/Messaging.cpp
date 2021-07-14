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

      static Configuration currentConfig;
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
        auto it = signals.find(std::make_pair(type, endPoint));
        if(it != signals.end())
          it->second(s);
      }

      static void createInChannels(const Configuration &conf)
      {
        inChannelConfig = conf.offerEndpoints;
        for(const auto &c : conf.offerEndpoints)
        {
          auto cb = [peer = c.peer](const auto &s) { notifyClients(s, peer); };

          parseURI(c.uri, [=](auto scheme, auto, auto, auto port) {
            nltools_assertOnDevPC(scheme == "ws");  // Currently, only web sockets are supported
            inChannels[c.peer] = std::make_unique<ws::WebSocketInChannel>(cb, port, c.prio);
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
            outChannels[c.peer] = std::make_unique<ws::WebSocketOutChannel>(
                host, port, c.prio, [peer = c.peer] { connectionSignals[peer](); });
          });
        }
      }

      static sigc::connection connectReceiver(MessageType type, EndPoint endPoint,
                                              std::function<void(const SerializedMessage &)> cb)
      {
        auto ret = signals[std::make_pair(type, endPoint)].connect(cb);
        return ret;
      }

      bool send(nltools::msg::EndPoint receiver, const SerializedMessage &msg)
      {
        try
        {
          return outChannels.at(receiver)->send(msg);
        }
        catch(...)
        {
          nltools::Log::warning("no such receiver:", toString(receiver));
          return false;
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

    ChannelConfiguration::ChannelConfiguration(EndPoint p, const std::string &hostName, threading::Priority prio)
        : peer(p)
        , uri(nltools::string::concat("ws://", hostName, ":", getPortFor(p)))
        , prio(prio)
    {
    }

    ChannelConfiguration::ChannelConfiguration(EndPoint p, threading::Priority prio)
        : ChannelConfiguration(p, "localhost", prio)
    {
    }

    void init(const Configuration &conf)
    {
      deInit();
      detail::currentConfig = conf;
      detail::createInChannels(conf);
      detail::createOutChannels(conf);
    }

    void deInit()
    {
      detail::currentConfig = {};
      detail::outChannels.clear();
      detail::inChannels.clear();
      detail::signals.clear();
      detail::connectionSignals.clear();
    }

    const Configuration &getConfig()
    {
      return detail::currentConfig;
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

    void flush(EndPoint receiver, const std::chrono::milliseconds timeout)
    {
      detail::outChannels.at(receiver)->flush(timeout);
    }

  }
}
