#pragma once

#include "Messaging.h"
#include <nltools/Types.h>
#include <cstring>

namespace nltools
{
  namespace msg
  {
    template <MessageType msgType> struct Message
    {
      static const MessageType theType = msgType;
      MessageType type = msgType;
    };

    struct ParameterChangedMessage : Message<MessageType::Parameter>
    {
      ParameterChangedMessage(uint16_t id = 0, float controlPosition = 0)
          : parameterId(id)
          , controlPosition(controlPosition)
      {
      }

      uint16_t parameterId;
      float controlPosition;
    };

    struct RotaryChangedMessage : Message<MessageType::RotaryChanged>
    {
      int8_t increment;
    };

    struct ButtonChangedMessage : Message<MessageType::ButtonChanged>
    {
      int8_t buttonId;
      bool pressed;
    };

    struct SetRibbonLEDMessage : Message<MessageType::SetRibbonLED>
    {
      uint8_t id;
      uint8_t brightness;
    };

    struct SetPanelLEDMessage : Message<MessageType::SetPanelLED>
    {
      uint8_t id;
      bool on;
    };

    struct SetOLEDMessage : Message<MessageType::SetOLED>
    {
      uint8_t pixels[256][96];
    };

    struct LPCMessage : Message<MessageType::LPC>
    {
      Glib::RefPtr<Glib::Bytes> message;
    };

    struct PingMessage : Message<MessageType::Ping>
    {
    };

    namespace detail
    {
      // default (de)serialization for messages, may be specialized for more compilcated types:
      template <> inline LPCMessage deserialize<LPCMessage>(const SerializedMessage &s)
      {
        LPCMessage ret;
        gsize numBytes = 0;
        auto data = reinterpret_cast<const uint8_t *>(s->get_data(numBytes));
        ret.message = Glib::Bytes::create(data + 2, numBytes - 2);
        return ret;
      }

      template <> inline SerializedMessage serialize<LPCMessage>(const LPCMessage &msg)
      {
        gsize numBytes = 0;
        auto data = reinterpret_cast<const uint8_t *>(msg.message->get_data(numBytes));
        auto scratch = reinterpret_cast<uint16_t *>(g_malloc(numBytes + 2));
        scratch[0] = static_cast<uint16_t>(MessageType::LPC);
	std::memcpy(&scratch[1], data, numBytes);
        auto bytes = g_bytes_new_take(scratch, numBytes + 2);
        return Glib::wrap(bytes);
      }
    }

    struct SetPresetMessage : Message<MessageType::Preset>
    {
      struct Parameter
      {
        int32_t id;
        float controlPosition = 0;
      };

      struct RibbonParameter : Parameter
      {
        RibbonTouchBehaviour ribbonTouchBehaviour;
        RibbonReturnMode ribbonReturnMode;
      };

      struct PedalParameter : Parameter
      {
        PedalModes pedalMode;
        ReturnMode returnMode;
      };

      struct AftertouchParameter : Parameter
      {
        ReturnMode returnMode;
      };

      struct BenderParameter : Parameter
      {
        ReturnMode returnMode;
      };

      struct MacroParameter : Parameter
      {
      };

      struct ModulateableParameter : Parameter
      {
        MacroControls mc = MacroControls::NONE;
        float modulationAmount = 0;
      };

      struct UnmodulatebaleParameter : Parameter
      {
      };

      std::array<PedalParameter, 4> pedals;
      std::array<AftertouchParameter, 1> aftertouch;
      std::array<RibbonParameter, 2> ribbons;
      std::array<BenderParameter, 1> bender;
      std::array<MacroParameter, 4> macros;
      std::array<ModulateableParameter, 89> modulateables;
      std::array<UnmodulatebaleParameter, 138> unmodulateables;
    };
  }
}
