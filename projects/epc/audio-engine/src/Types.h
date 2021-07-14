#pragma once

#include <cstdint>
#include <chrono>

using Sample = float;
using SamplePosition = uint64_t;

struct MidiEvent
{
  uint8_t raw[3] { 0x0 };
  std::chrono::high_resolution_clock::time_point timestamp;
};

enum class DecoderEventType
{
  KeyDown,
  KeyUp,
  HardwareChange,
  UNKNOWN
};

struct SampleFrame
{
  Sample left;
  Sample right;
};

using SampleInt16 = int16_t;
using SampleInt32 = int32_t;
using SampleFloat32 = float;

using FrameId = uint64_t;