#pragma once

#include <memory>
#include <giomm.h>
#include <sigc++/sigc++.h>
#include <assert.h>
#include <cstring>

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

      TestEndPoint,
    };

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
      Request,

      // messages to be used from v1.7 on
      SetRibbonLED,
      SetPanelLED,
      SetOLED,
      RotaryChanged,
      ButtonChanged,
      LPC,
      Ping
    };

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
      void send(EndPoint receiver, SerializedMessage msg);

      template <typename Msg>
      sigc::connection receive(MessageType type, EndPoint receivingEndPoint, std::function<void(const Msg &)> cb)
      {
        return receiveSerialized(type, receivingEndPoint,
                                 [=](const SerializedMessage &s) { cb(detail::deserialize<Msg>(s)); });
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
