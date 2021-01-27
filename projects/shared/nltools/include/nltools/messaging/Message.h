#pragma once

#include "Messaging.h"
#include <cstring>
#include <nltools/Testing.h>
#include <nltools/Types.h>

namespace nltools
{
  namespace msg
  {
    namespace Midi
    {
      struct SimpleMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::MidiSimpleMessage;
        }

        SimpleMessage()
        {
        }

        SimpleMessage(uint8_t status, uint8_t data)
        {
          rawBytes[0] = status;
          rawBytes[1] = data;
          numBytesUsed = 2;
        }

        SimpleMessage(uint8_t status, uint8_t data1, uint8_t data2)
        {
          rawBytes[0] = status;
          rawBytes[1] = data1;
          rawBytes[2] = data2;
          numBytesUsed = 3;
        }

        std::array<uint8_t, 3> rawBytes;
        uint8_t numBytesUsed = 0;
      };

      struct ProgramChangeMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::MidiProgramChange;
        }

        uint8_t program;
      };
    }

    using tID = int;
    using tControlPosition = double;

    namespace Keyboard
    {
      struct NoteDown
      {
        constexpr static MessageType getType()
        {
          return MessageType::NoteDown;
        }

        int m_keyPos;
      };

      struct NoteUp
      {
        constexpr static MessageType getType()
        {
          return MessageType::NoteUp;
        }

        int m_keyPos;
      };
    }

    namespace USB
    {
      struct USBStatusMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::USBStatusMessage;
        }

        bool m_usbAvailable;
        bool m_updateAvailable;
      };
    }

    namespace FileSystem
    {
      struct Sync
      {
        constexpr static MessageType getType()
        {
          return MessageType::SyncFS;
        }
      };
    }

    namespace WiFi
    {

      namespace Helper
      {
        template <size_t tSize> struct StringWrapper
        {
          StringWrapper()
          {
            std::memset(data, '\0', tSize + 1);
          }

          void set(const std::string& s)
          {
            assert(s.size() <= tSize);
            std::copy(s.begin(), s.end() + 1, data);
          }

          std::string get() const
          {
            return data;
          }

          char data[tSize + 1];
        };
      }

      struct SetWiFiSSIDMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::WiFiSetSSID;
        }

        SetWiFiSSIDMessage()
        {
        }

        template <typename T> SetWiFiSSIDMessage(const T& ssid)
        {
          m_ssid.set(ssid);
        }

        Helper::StringWrapper<128> m_ssid;
      };

      struct SetWiFiPasswordMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::WiFiSetPassword;
        }

        SetWiFiPasswordMessage()
        {
        }

        template <typename T> SetWiFiPasswordMessage(const T& password)
        {
          m_password.set(password);
        }

        Helper::StringWrapper<8> m_password;
      };
    }

    namespace Setting
    {
      struct NoteShiftMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::NoteShiftSetting;
        }

        int m_shift;
      };

      struct PresetGlitchMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::PresetGlitchSetting;
        }

        bool m_enabled;
      };

      struct TuneReference
      {
        constexpr static MessageType getType()
        {
          return MessageType::TuneReference;
        }

        double m_tuneReference;
      };

      struct MidiBridgeSettings
      {
        constexpr static MessageType getType()
        {
          return MessageType::MidiBridgeSettings;
        }

        bool enable = false;
      };

      struct MidiSettingsMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::MidiSettings;
        }

        int receiveChannel = 0;       // 0-15, -1 => None, 16 => Omni/All
        int receiveSplitChannel = 0;  // 0-15, -1 => None, 16 => Omni/All

        int sendChannel = 0;       // 0-15, -1 => None
        int sendSplitChannel = 0;  //0-15, -1 => None

        bool receiveProgramChange = false;
        bool receiveNotes = false;
        bool receiveControllers = false;

        bool sendProgramChange = false;
        bool sendNotes = false;
        bool sendControllers = false;

        bool localNotes = false;
        bool localControllers = false;
      };

      struct TransitionTimeMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::TransitionTimeSetting;
        }

        float m_value;
      };

      struct EditSmoothingTimeMessage
      {
        constexpr static MessageType getType()
        {
          return MessageType::EditSmoothingTimeSetting;
        }

        float m_time;
      };
    }

    struct HWSourceChangedMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::HWSourceParameter;
      }

      tID parameterId;
      tControlPosition controlPosition;
      ReturnMode returnMode;
    };

    struct HWAmountChangedMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::HWAmountParameter;
      }

      tID parameterId;
      tControlPosition controlPosition;
    };

    struct MacroControlChangedMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::MacroControlParameter;
      }

      tID parameterId;
      tControlPosition controlPosition;
    };

    struct UnmodulateableParameterChangedMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::UnmodulateableParameter;
      }

      tID parameterId;
      tControlPosition controlPosition;
      VoiceGroup voiceGroup;
    };

    struct ModulateableParameterChangedMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::ModulateableParameter;
      }

      tID parameterId;
      tControlPosition controlPosition;
      MacroControls sourceMacro;
      tControlPosition mcAmount;
      tControlPosition mcUpper;
      tControlPosition mcLower;
      VoiceGroup voiceGroup;
    };

    struct RotaryChangedMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::RotaryChanged;
      }

      int8_t increment;
    };

    struct TimestampedRotaryChangedMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::TimestampedRotaryChanged;
      }

      int8_t increment;
      uint64_t timestamp;
    };

    struct ButtonChangedMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::ButtonChanged;
      }

      int8_t buttonId;
      bool pressed;
    };

    struct SetRibbonLEDMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::SetRibbonLED;
      }

      uint8_t id;
      uint8_t brightness;
    };

    struct SetPanelLEDMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::SetPanelLED;
      }

      uint8_t id;
      bool on;
    };

    struct SetOLEDMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::SetOLED;
      }

      uint8_t pixels[256][96];
    };

    struct SetTimestampedOledMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::SetOLEDTimestamped;
      }

      SetOLEDMessage m_oledMessage;
      int64_t m_timestamp;
    };

    struct PlaycontrollerMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::Playcontroller;
      }

      Glib::RefPtr<Glib::Bytes> message;
    };

    struct PingMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::Ping;
      }
    };

    struct HardwareSourceChangedNotification
    {
      constexpr static MessageType getType()
      {
        return MessageType::NotifyHardwareSourceChanged;
      }

      size_t hwSource = 0;  // 0...7
      double position = 0;  // -1...1
    };

    namespace detail
    {
      template <> inline PlaycontrollerMessage deserialize<PlaycontrollerMessage>(const SerializedMessage& s)
      {
        PlaycontrollerMessage ret;
        gsize numBytes = 0;
        auto data = reinterpret_cast<const uint8_t*>(s->get_data(numBytes));
        ret.message = Glib::Bytes::create(data + 2, numBytes - 2);
        return ret;
      }

      template <> inline SerializedMessage serialize<PlaycontrollerMessage>(const PlaycontrollerMessage& msg)
      {
        gsize numBytes = 0;
        auto data = reinterpret_cast<const uint8_t*>(msg.message->get_data(numBytes));
        auto scratch = reinterpret_cast<uint16_t*>(g_malloc(numBytes + 2));
        scratch[0] = static_cast<uint16_t>(MessageType::Playcontroller);
        std::memcpy(&scratch[1], data, numBytes);
        auto bytes = g_bytes_new_take(scratch, numBytes + 2);
        return Glib::wrap(bytes);
      }
    }

    namespace ParameterGroups
    {
      struct Parameter
      {
        uint16_t id {};
        double controlPosition = 0;
      };

      struct RibbonParameter : Parameter
      {
        RibbonTouchBehaviour ribbonTouchBehaviour {};
        RibbonReturnMode ribbonReturnMode {};
      };

      struct PedalParameter : Parameter
      {
        PedalModes pedalMode {};
        ReturnMode returnMode {};
      };

      struct MacroParameter : Parameter
      {
      };

      struct ModulateableParameter : Parameter
      {
        MacroControls mc = MacroControls::NONE;
        double modulationAmount = 0;
      };

      struct UnmodulateableParameter : Parameter
      {
      };

      struct GlobalParameter : Parameter
      {
      };

      struct HardwareSourceParameter : Parameter
      {
        ReturnMode returnMode = ReturnMode::None;
      };

      struct HardwareAmountParameter : Parameter
      {
      };

      struct SplitPoint : ModulateableParameter
      {
      };

      struct UnisonGroup
      {
        ParameterGroups::UnmodulateableParameter unisonVoices;
        ParameterGroups::ModulateableParameter detune;
        ParameterGroups::UnmodulateableParameter phase;
        ParameterGroups::UnmodulateableParameter pan;
      };

      struct MonoGroup
      {
        ParameterGroups::UnmodulateableParameter monoEnable;
        ParameterGroups::UnmodulateableParameter legato;
        ParameterGroups::UnmodulateableParameter priority;
        ParameterGroups::ModulateableParameter glide;
      };

      struct MasterGroup
      {
        ParameterGroups::ModulateableParameter volume;
        ParameterGroups::ModulateableParameter tune;
      };

      inline bool operator==(const Parameter& lhs, const Parameter& rhs)
      {
        auto ret = lhs.id == rhs.id;
        ret &= lhs.controlPosition == rhs.controlPosition;
        return ret;
      }

      inline bool operator==(const ModulateableParameter& lhs, const ModulateableParameter& rhs)
      {
        auto ret = lhs.id == rhs.id;
        ret &= lhs.controlPosition == rhs.controlPosition;
        ret &= lhs.modulationAmount == rhs.modulationAmount;
        ret &= lhs.mc == rhs.mc;
        return ret;
      }

      inline bool operator==(const MonoGroup& lhs, const MonoGroup& rhs)
      {
        auto ret = lhs.glide == rhs.glide;
        ret &= lhs.monoEnable == rhs.monoEnable;
        ret &= lhs.priority == rhs.priority;
        ret &= lhs.legato == rhs.legato;
        return ret;
      }

      inline bool operator==(const UnisonGroup& lhs, const UnisonGroup& rhs)
      {
        auto ret = lhs.unisonVoices == rhs.unisonVoices;
        ret &= lhs.detune == rhs.detune;
        ret &= lhs.pan == rhs.pan;
        ret &= lhs.phase == rhs.phase;
        return ret;
      }

      inline bool operator==(const MasterGroup& lhs, const MasterGroup& rhs)
      {
        auto ret = lhs.volume == rhs.volume;
        ret &= lhs.tune == rhs.tune;
        return ret;
      }
    }

    struct SinglePresetMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::SinglePreset;
      }

      std::array<ParameterGroups::MacroParameter, 6> macros;
      std::array<ParameterGroups::UnmodulateableParameter, 6> macrotimes;

      std::array<ParameterGroups::ModulateableParameter, 101> modulateables;
      std::array<ParameterGroups::UnmodulateableParameter, 94> unmodulateables;

      std::array<ParameterGroups::HardwareSourceParameter, 8> hwsources;
      std::array<ParameterGroups::HardwareAmountParameter, 48> hwamounts;

      ParameterGroups::UnisonGroup unison;
      ParameterGroups::MonoGroup mono;

      ParameterGroups::MasterGroup master;

      std::array<ParameterGroups::GlobalParameter, 13> scale;
    };

    inline bool operator==(const SinglePresetMessage& lhs, const SinglePresetMessage& rhs)
    {
      auto ret = lhs.unmodulateables == rhs.unmodulateables;
      ret &= lhs.modulateables == rhs.modulateables;
      ret &= lhs.scale == rhs.scale;
      ret &= lhs.mono == rhs.mono;
      ret &= lhs.unison == rhs.unison;
      ret &= lhs.master == rhs.master;
      ret &= lhs.hwamounts == rhs.hwamounts;
      ret &= lhs.hwsources == rhs.hwsources;
      ret &= lhs.macros == rhs.macros;
      ret &= lhs.macrotimes == rhs.macrotimes;
      return ret;
    }

    inline bool operator!=(const SinglePresetMessage& lhs, const SinglePresetMessage& rhs)
    {
      return !(lhs == rhs);
    }

    struct SplitPresetMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::SplitPreset;
      }

      std::array<std::array<ParameterGroups::ModulateableParameter, 101>, 2> modulateables;
      std::array<std::array<ParameterGroups::UnmodulateableParameter, 94>, 2> unmodulateables;

      std::array<ParameterGroups::UnisonGroup, 2> unison;
      std::array<ParameterGroups::MonoGroup, 2> mono;

      ParameterGroups::MasterGroup master;

      std::array<ParameterGroups::HardwareSourceParameter, 8> hwsources;
      std::array<ParameterGroups::HardwareAmountParameter, 48> hwamounts;

      std::array<ParameterGroups::MacroParameter, 6> macros;
      std::array<ParameterGroups::UnmodulateableParameter, 6> macrotimes;

      std::array<ParameterGroups::GlobalParameter, 13> scale;
      std::array<ParameterGroups::SplitPoint, 2> splitpoint;
    };

    inline bool operator==(const SplitPresetMessage& lhs, const SplitPresetMessage& rhs)
    {
      auto ret = lhs.unmodulateables == rhs.unmodulateables;
      ret &= lhs.modulateables == rhs.modulateables;
      ret &= lhs.scale == rhs.scale;
      ret &= lhs.mono == rhs.mono;
      ret &= lhs.unison == rhs.unison;
      ret &= lhs.master == rhs.master;
      ret &= lhs.hwamounts == rhs.hwamounts;
      ret &= lhs.hwsources == rhs.hwsources;
      ret &= lhs.macros == rhs.macros;
      ret &= lhs.macrotimes == rhs.macrotimes;
      ret &= lhs.splitpoint == rhs.splitpoint;
      return ret;
    }

    inline bool operator!=(const SplitPresetMessage& lhs, const SplitPresetMessage& rhs)
    {
      return !(lhs == rhs);
    }

    struct LayerPresetMessage
    {
      constexpr static MessageType getType()
      {
        return MessageType::LayerPreset;
      }

      std::array<ParameterGroups::HardwareSourceParameter, 8> hwsources;
      std::array<ParameterGroups::HardwareAmountParameter, 48> hwamounts;

      std::array<ParameterGroups::MacroParameter, 6> macros;
      std::array<ParameterGroups::UnmodulateableParameter, 6> macrotimes;

      std::array<std::array<ParameterGroups::ModulateableParameter, 101>, 2> modulateables;
      std::array<std::array<ParameterGroups::UnmodulateableParameter, 94>, 2> unmodulateables;

      ParameterGroups::UnisonGroup unison;
      ParameterGroups::MonoGroup mono;

      ParameterGroups::MasterGroup master;

      std::array<ParameterGroups::GlobalParameter, 13> scale;
    };

    inline bool operator==(const LayerPresetMessage& lhs, const LayerPresetMessage& rhs)
    {
      auto ret = lhs.unmodulateables == rhs.unmodulateables;
      ret &= lhs.modulateables == rhs.modulateables;
      ret &= lhs.scale == rhs.scale;
      ret &= lhs.mono == rhs.mono;
      ret &= lhs.unison == rhs.unison;
      ret &= lhs.master == rhs.master;
      ret &= lhs.hwamounts == rhs.hwamounts;
      ret &= lhs.hwsources == rhs.hwsources;
      ret &= lhs.macros == rhs.macros;
      ret &= lhs.macrotimes == rhs.macrotimes;
      return ret;
    }

    inline bool operator!=(const LayerPresetMessage& lhs, const LayerPresetMessage& rhs)
    {
      return !(lhs == rhs);
    }
  }
}
