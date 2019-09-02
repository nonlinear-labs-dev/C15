#pragma once

#include <memory>
#include <giomm.h>
#include <sigc++/sigc++.h>
#include <assert.h>
#include <cstring>
#include <nltools/logging/Log.h>

namespace nltools
{
  namespace msg
  {
    // Types:
    using SerializedMessage = Glib::RefPtr<Glib::Bytes>;

    enum class EndPoint
    {
      None,
      Lpc,
      Oled,
      PanelLed,
      RibbonLed,
      AudioEngine,
      Playground,
      TestEndPoint
    };

    inline std::string toStringEndPoint(const EndPoint& e) {
      switch(e) {
        case EndPoint::None:
          return "EndPoint::None";
        case EndPoint::Lpc:
          return "EndPoint::Lpc";
        case EndPoint::Oled:
          return "EndPoint::Oled";
        case EndPoint::PanelLed:
          return "EndPoint::PanelLed";
        case EndPoint::RibbonLed:
          return "EndPoint::RibbonLed";
        case EndPoint::AudioEngine:
          return "EndPoint::AudioEngine";
        case EndPoint::Playground:
          return "EndPoint::Playground";
        case EndPoint::TestEndPoint:
          return "EndPoint::TestEndPoint";
      }
      return "";
    }

    uint getPortFor(EndPoint p);

    enum class MessageType : uint16_t
    {
      // deprecated messages for LPC <-> Playground
      Preset,
      Morph_A,
      Morph_B,
      Parameter,
      EditControl,
      MorphPosition,
      Setting,
      Notification,
      Assertion,
      Request, //9

      // messages to be used from v1.7 on
      SetRibbonLED, //10
      SetPanelLED, //11
      SetOLED, //12
      RotaryChanged,
      ButtonChanged,
      LPC,
      Ping
    };

    inline std::string toStringMessageType(const MessageType &type)
    {
      switch(type) {
        case MessageType::Preset:
          return "MessageType::Preset";
        case MessageType::Morph_A:
          return "MessageType::Morph_A";
        case MessageType::Morph_B:
          return "MessageType::Morph_B";
        case MessageType::Parameter:
          return "MessageType::Parameter";
        case MessageType::EditControl:
          return "MessageType::EditControl";
        case MessageType::MorphPosition:
          return "MessageType::MorphPosition";
        case MessageType::Setting:
          return "MessageType::Setting";
        case MessageType::Notification:
          return "MessageType::Notification";
        case MessageType::Assertion:
          return "MessageType::Assertion";
        case MessageType::Request:
          return "MessageType::Request";
        case MessageType::SetRibbonLED:
          return "MessageType::SetRibbonLED";
        case MessageType::SetPanelLED:
          return "MessageType::SetPanelLED";
        case MessageType::SetOLED:
          return "MessageType::SetOLED";
        case MessageType::RotaryChanged:
          return "MessageType::RotaryChanged";
        case MessageType::ButtonChanged:
          return "MessageType::ButtonChanged";
        case MessageType::LPC:
          return "MessageType::LPC";
        case MessageType::Ping:
          return "MessageType::Ping";
      }
      return "";
    }

    namespace detail
    {
      // default (de)serialization for messages, may be specialized for more compilcated types:
      template <typename Msg> Msg deserialize(const SerializedMessage &s)
      {
        Msg ret;
        assert(s->get_size() == sizeof(Msg));
        gsize numBytes = 0;
        memcpy(&ret, s->get_data(numBytes), sizeof(Msg));
        return ret;
      }

      template <typename Msg> SerializedMessage serialize(const Msg &msg)
      {
        return Glib::Bytes::create(&msg, sizeof(Msg));
      }

      // send raw bytes to receiver
      void send(EndPoint receiver, const SerializedMessage& msg);

      template <typename Msg>
      sigc::connection receive(MessageType type, EndPoint receivingEndPoint, std::function<void(const Msg &)> cb)
      {
        return receiveSerialized(type, receivingEndPoint, [=](const SerializedMessage &s) {
          auto msg = detail::deserialize<Msg>(s);
          cb(msg);
        });
      }

      sigc::connection receiveSerialized(MessageType type, EndPoint receivingEndPoint,
                                         std::function<void(const SerializedMessage &)> cb);
    }

    // Client has to call this on startup

    struct ChannelConfiguration
    {
      ChannelConfiguration(EndPoint p = EndPoint::None);
      ChannelConfiguration(EndPoint p, const std::string &hostName);
      EndPoint peer;
      std::string uri;
    };

    struct Configuration
    {
      std::vector<ChannelConfiguration> offerEndpoints;
      std::vector<ChannelConfiguration> useEndpoints;
    };

    void init(const Configuration &conf);
    void deInit();

    // wait at most timeOut for the connection to be established
    // return true if there is a connection to receiver
    bool waitForConnection(EndPoint receiver, std::chrono::milliseconds timeOut = std::chrono::seconds(10));

    // Send msg to receiver. If there is no receiver, does nothing.
    template <typename Msg> void send(EndPoint receiver, const Msg &msg)
    {
      detail::send(receiver, detail::serialize<Msg>(msg));
    }

    template <typename Msg> sigc::connection receive(EndPoint receivingEndPoint, std::function<void(const Msg &)> cb)
    {
      return detail::receive<Msg>(Msg::theType, receivingEndPoint, [=](const auto &s) { cb(s); });
    }

    sigc::connection onConnectionEstablished(EndPoint endPoint, std::function<void()> cb);
  }
}
