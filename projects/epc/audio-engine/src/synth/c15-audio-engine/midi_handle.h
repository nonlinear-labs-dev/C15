#pragma once

#include <stdint.h>
#include <parameter_declarations.h>
#include <nltools/Types.h>

namespace Midi
{

  namespace Channel
  {
    inline int statusToChannel(uint8_t status)
    {
      return status & 0b00001111;
    }
  }

  namespace MSB
  {
    // encode HW source number and MSB CC
    enum HWSourceMidiCC
    {
      Ped1 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Pedal_1) << 8 | 20,
      Ped2 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Pedal_2) << 8 | 21,
      Ped3 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Pedal_3) << 8 | 22,
      Ped4 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Pedal_4) << 8 | 23,
      Bender = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Bender) << 8 | 0,
      Aftertouch = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Aftertouch) << 8 | 0,
      Rib1 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Ribbon_1) << 8 | 24,
      Rib2 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Ribbon_2) << 8 | 25
    };
  }  // namespace Midi::MSB

  namespace LSB
  {
    // encode HW source number and LSB CC
    enum HWSourceMidiCC
    {
      Ped1 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Pedal_1) << 8 | 52,
      Ped2 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Pedal_2) << 8 | 53,
      Ped3 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Pedal_3) << 8 | 54,
      Ped4 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Pedal_4) << 8 | 55,
      Rib1 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Ribbon_1) << 8 | 56,
      Rib2 = static_cast<uint16_t>(C15::Parameters::Hardware_Sources::Ribbon_2) << 8 | 57,
      Vel = 0xFF << 8 | 88
    };
  }  // namespace Midi::LSB

  // CC retrieval

  template <typename CC> struct CC_getter
  {
    template <CC ID> static inline constexpr uint32_t getCC()
    {
      return static_cast<uint32_t>(ID) & 0xFF;  // this needs to be 32 bit?
    }
  };

  using getMSB = CC_getter<MSB::HWSourceMidiCC>;
  using getLSB = CC_getter<LSB::HWSourceMidiCC>;

  // encoding, decoding

  enum class Formats
  {
    _7_Bits_ = 7,
    _14_Bits_ = 14
  };

  template <Formats Bits> struct FullCCRange
  {
    static constexpr uint16_t bottomRange = 1 << (static_cast<uint16_t>(Bits) - 1);
    static constexpr uint16_t fullRange = bottomRange << 1;

    static constexpr uint16_t splitPosition = bottomRange + 1;

    static constexpr uint16_t topRange = (bottomRange - 1) << 1;
    static constexpr uint16_t topOffset = 1;

    static constexpr float bottomDenom = 1.0f / (static_cast<float>(fullRange));
    static constexpr float topDenom = 1.0f / (static_cast<float>(topRange));

    static float decodeUnipolarMidiValue(uint16_t _midiValue)
    {
      if(_midiValue < splitPosition)
      {
        return static_cast<float>(_midiValue) * bottomDenom;
      }
      else
      {
        return static_cast<float>(_midiValue - topOffset) * topDenom;
      }
    }

    static float decodeBipolarMidiValue(uint16_t _midiValue)
    {
      return (2.0f * decodeUnipolarMidiValue(_midiValue)) - 1.0f;
    }

    static uint16_t encodeUnipolarMidiValue(float _controlPosition)
    {
      if(_controlPosition > 0.5f)
      {
        return topOffset
            + static_cast<uint16_t>(static_cast<float>(topRange) * (_controlPosition < 1.0f ? _controlPosition : 1.0f));
      }
      else
      {
        return static_cast<uint16_t>(static_cast<float>(fullRange)
                                     * (_controlPosition > 0.0f ? _controlPosition : 0.0f));
      }
    }

    static uint16_t encodeBipolarMidiValue(float _controlPosition)
    {
      return encodeUnipolarMidiValue(0.5f + (0.5f * _controlPosition));
    }
  };

  struct clipped14BitCCRange
  {
    static constexpr uint16_t bottomRange = 1 << (static_cast<uint16_t>(Formats::_14_Bits_) - 1);
    static constexpr uint16_t fullRange = bottomRange << 1;

    static constexpr uint16_t splitPosition = bottomRange + 1;

    static constexpr uint16_t topRange = (bottomRange - 128) << 1;
    static constexpr uint16_t topOffset = 128;

    static constexpr uint16_t clipPosition = fullRange - topOffset + 1;

    static constexpr float bottomDenom = 1.0f / (static_cast<float>(fullRange));
    static constexpr float topDenom = 1.0f / (static_cast<float>(topRange));

    static float decodeUnipolarMidiValue(uint16_t _midiValue)
    {
      if(_midiValue < splitPosition)
      {
        return static_cast<float>(_midiValue) * bottomDenom;
      }
      else if(_midiValue < clipPosition)
      {
        return static_cast<float>(_midiValue - topOffset) * topDenom;
      }
      else
      {
        return 1.0f;
      }
    }

    static float decodeBipolarMidiValue(uint16_t _midiValue)
    {
      return (2.0f * decodeUnipolarMidiValue(_midiValue)) - 1.0f;
    }

    static uint16_t encodeUnipolarMidiValue(float _controlPosition)
    {
      if(_controlPosition > 0.5f)
      {
        return topOffset
            + static_cast<uint16_t>(static_cast<float>(topRange) * (_controlPosition < 1.0f ? _controlPosition : 1.0f));
      }
      else
      {
        return static_cast<uint16_t>(static_cast<float>(fullRange)
                                     * (_controlPosition > 0.0f ? _controlPosition : 0.0f));
      }
    }

    static uint16_t encodeBipolarMidiValue(float _controlPosition)
    {
      return encodeUnipolarMidiValue(0.5f + (0.5f * _controlPosition));
    }
  };

  struct clipped14BitVelRange
  {
    static constexpr uint16_t BottomOffset = 128;
    static constexpr uint16_t TopOffset = 128;
    static constexpr uint16_t ValidRange = (1 << static_cast<uint16_t>(Formats::_14_Bits_)) - BottomOffset - TopOffset;
    static constexpr float Denom = 1.0f / (static_cast<float>(ValidRange));

    static bool isValidNoteOnVelocity(const uint16_t& _midiValue)
    {
      return _midiValue >= BottomOffset;
    }

    static float decodeUnipolarMidiValue(const uint16_t& _midiValue)
    {
      if(isValidNoteOnVelocity(_midiValue))
      {
        const uint16_t val = _midiValue - BottomOffset;
        return val < ValidRange ? (static_cast<float>(val) * Denom) : 1.0f;
      }
      return 0.0f;
    }
    static uint16_t encodeUnipolarMidiValue(const float& _controlPosition)
    {
      if(_controlPosition < 0.0f)
      {
        return 0;
      }
      else
      {
        const float val = _controlPosition > 1.0f ? 1.0f : _controlPosition;
        return BottomOffset + static_cast<uint16_t>(val * static_cast<float>(ValidRange));
      }
    }
  };

}  // namespace Midi
