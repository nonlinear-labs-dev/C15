#pragma once

namespace TCD_HELPER
{
  enum class TCD_HW_IDS : uint8_t
  {
    Pedal1 = 0,
    Pedal2 = 1,
    Pedal3 = 2,
    Pedal4 = 3,
    Bender = 4,
    Aftertouch = 5,
    Ribbon1 = 6,
    Ribbon2 = 7
  };

  enum class TCD_PROTOCOL : uint8_t
  {
    BASE = 0b11100000,
    TCD_KEY_POS = 13,
    TCD_KEY_DOWN = 14,
    TCD_KEY_UP = 15,
    TCD_UNUSED_VAL = 0x00
  };


  inline uint8_t to(TCD_PROTOCOL p)
  {
    return static_cast<uint8_t>(p);
  }

  inline uint8_t to(TCD_HW_IDS h)
  {
    return static_cast<uint8_t>(h);
  }

  inline MidiEvent createKeyPosEvent(int notePos)
  {
    uint8_t type = to(TCD_PROTOCOL::BASE) | to(TCD_PROTOCOL::TCD_KEY_POS);
    return { type, to(TCD_PROTOCOL::TCD_UNUSED_VAL), static_cast<uint8_t>(notePos) };
  }

  inline MidiEvent createKeyDownEvent(int val1, int val2)
  {
    uint8_t type = to(TCD_PROTOCOL::BASE) | to(TCD_PROTOCOL::TCD_KEY_DOWN);
    return { type, static_cast<uint8_t>(val1), static_cast<uint8_t>(val2) };
  }

  inline MidiEvent createKeyUpEvent(int val1, int val2)
  {
    uint8_t type = to(TCD_PROTOCOL::BASE) | to(TCD_PROTOCOL::TCD_KEY_UP);
    return { type, static_cast<uint8_t>(val1), static_cast<uint8_t>(val2) };
  }

  inline MidiEvent createFullPressureHWEvent(TCD_HW_IDS id)
  {
    constexpr static auto SixteenThousand = 0b11111010000000;
    uint8_t type = to(TCD_PROTOCOL::BASE) | to(id);
    return { type, (uint8_t) (SixteenThousand >> 7), (uint8_t) (SixteenThousand & 127) };
  }
}