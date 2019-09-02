#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/websocket/WebSocketOutChannel.h>
#include <nltools/messaging/websocket/WebSocketInChannel.h>
#include <nltools/logging/Log.h>
#include <nltools/ExceptionTools.h>
#include <nltools/StringTools.h>
#include <memory>
#include <map>
#include <list>

namespace nltools {
  namespace msg {
    namespace detail {
      using OutChannels = std::map<EndPoint, std::unique_ptr<OutChannel>>;
      using InChannels = std::map<EndPoint, std::unique_ptr<InChannel>>;
      using Signals = std::map<std::pair<MessageType, EndPoint>, sigc::signal<void, const SerializedMessage &>>;
      using ConnectionSignals = std::map<EndPoint, sigc::signal<void>>;

      static OutChannels outChannels;
      static InChannels inChannels;
      static Signals signals;
      static ConnectionSignals connectionSignals;

      static void notifyClients(const SerializedMessage &s, EndPoint endPoint) {
        gsize numBytes = 0;
        auto data = reinterpret_cast<const uint16_t *>(s->get_data(numBytes));
        auto type = static_cast<MessageType>(data[0]);

        try {
          signals.at(std::make_pair(type, endPoint))(s);
        } catch(...) {
          if(type != MessageType::Ping)
            nltools::Log::error("Could not find", toStringMessageType(type), "handler in", toStringEndPoint(endPoint));
        }
      }

      static void createInChannels(const Configuration &conf) {
        for (const auto &c : conf.offerEndpoints) {
          auto cb = [peer = c.peer](const auto &s) { notifyClients(s, peer); };

          parseURI(c.uri, [=](auto scheme, auto, auto, auto port) {
            assert(scheme == "ws");  // Currently, only web sockets are supported
            inChannels[c.peer] = std::make_unique<ws::WebSocketInChannel>(cb, port);
          });
        }
      }

      static void createOutChannels(const Configuration &conf) {
        for (const auto &c : conf.useEndpoints) {
          parseURI(c.uri, [=](auto scheme, auto host, auto, auto port) {
            assert(scheme == "ws");  // Currently, only web sockets are supported
            outChannels[c.peer] = std::make_unique<ws::WebSocketOutChannel>(host, port);
            outChannels[c.peer]->onConnectionEstablished([peer = c.peer] { connectionSignals[peer](); });
          });
        }
      }

      static sigc::connection connectReceiver(MessageType type, EndPoint endPoint,
                                              std::function<void(const SerializedMessage &)> cb) {
        auto ret = signals[std::make_pair(type, endPoint)].connect(cb);
        return ret;
      }

      void send(nltools::msg::EndPoint receiver, const SerializedMessage& msg) {
        outChannels.at(receiver)->send(msg);
      }

      sigc::connection receiveSerialized(MessageType type, EndPoint receivingEndPoint,
                                         std::function<void(const SerializedMessage &)> cb) {
        return connectReceiver(type, receivingEndPoint, cb);
      }
    }

    bool waitForConnection(EndPoint receiver, std::chrono::milliseconds timeOut) {
      return detail::outChannels.at(receiver)->waitForConnection(timeOut);
    }

    ChannelConfiguration::ChannelConfiguration(EndPoint p)
        : peer(p), uri(concat("ws://", "localhost", ":", getPortFor(p))) {
    }

    ChannelConfiguration::ChannelConfiguration(EndPoint p, const std::string &hostName)
        : peer(p), uri(concat("ws://", hostName, ":", getPortFor(p))) {
    }

    void init(const Configuration &conf) {
      deInit();
      detail::createInChannels(conf);
      detail::createOutChannels(conf);
    }

    void deInit() {
      detail::outChannels.clear();
      detail::inChannels.clear();
      detail::signals.clear();
      detail::connectionSignals.clear();
    }

    uint getPortFor(EndPoint p) {
      return static_cast<uint>(p) + 40100;
    }

    sigc::connection onConnectionEstablished(EndPoint endPoint, std::function<void()> cb) {
      auto ret = detail::connectionSignals[endPoint].connect(cb);

      if (detail::outChannels.at(endPoint)->isConnected())
        cb();

      return ret;
    }
  }
}
