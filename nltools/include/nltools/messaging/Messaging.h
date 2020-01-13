#pragma once

#include "nltools/Assert.h"
#include <memory>
#include <giomm.h>
#include <sigc++/sigc++.h>
#include <cstring>
#include <nltools/logging/Log.h>
#include <nltools/enums/EnumTools.h>

namespace nltools
{
  namespace msg
  {
    // Types:
    using SerializedMessage = Glib::RefPtr<Glib::Bytes>;

    ENUM(EndPoint, uint16_t, None, Lpc, Oled, PanelLed, RibbonLed, AudioEngine, Playground, TestEndPoint);

    uint getPortFor(EndPoint p);

    ENUM(MessageType, uint16_t, Preset, Morph_A, Morph_B, Parameter, EditControl, MorphPosition, Setting, Notification,
         Assertion, Request,

         SetRibbonLED, SetPanelLED, SetOLED, SetOLEDTimestamped, RotaryChanged, TimestampedRotaryChanged, ButtonChanged,
         LPC, Ping,

         SinglePreset, LayerPreset, SplitPreset, UnmodulateableParameter, ModulateableParameter, MacroControlParameter,
         HWAmountParameter, HWSourceParameter,

         NoteShiftSetting, PresetGlitchSetting, TransitionTimeSetting, EditSmoothingTimeSetting, TuneReference,

         NoteDown, NoteUp);

    namespace detail
    {
      // default (de)serialization for messages, may be specialized for more compilcated types:
      template <typename Msg> Msg deserialize(const SerializedMessage &s)
      {
        Msg ret;
        nltools_assertAlways(s->get_size() == sizeof(Msg) + 2);
        gsize numBytes = 0;
        auto ptr = reinterpret_cast<const uint8_t *>(s->get_data(numBytes));
        memcpy(&ret, ptr + 2, sizeof(Msg));
        return ret;
      }

      template <typename Msg> SerializedMessage serialize(const Msg &msg)
      {
        uint8_t scratch[sizeof(Msg) + 2];
        auto type = Msg::getType();
        memcpy(scratch, &type, 2);
        memcpy(scratch + 2, &msg, sizeof(Msg));
        return Glib::Bytes::create(scratch, sizeof(scratch));
      }

      // send raw bytes to receiver
      void send(EndPoint receiver, const SerializedMessage &msg);

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
    Configuration swapConfig(const Configuration &conf);
    void deInit();

    // wait at most timeOut for the sigc::connection to be established
    // return true if there is a sigc::connection to receiver
    bool waitForConnection(EndPoint receiver, std::chrono::milliseconds timeOut = std::chrono::seconds(10));

    // Send msg to receiver. If there is no receiver, does nothing.
    template <typename Msg> void send(EndPoint receiver, const Msg &msg)
    {
      detail::send(receiver, detail::serialize<Msg>(msg));
    }

    template <typename Msg> sigc::connection receive(EndPoint receivingEndPoint, std::function<void(const Msg &)> cb)
    {
      return detail::receive<Msg>(Msg::getType(), receivingEndPoint, [=](const auto &s) { cb(s); });
    }

    sigc::connection onConnectionEstablished(EndPoint endPoint, std::function<void()> cb);
  }
}
