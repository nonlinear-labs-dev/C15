#pragma once

#include <stdint.h>
#include <chrono>

using Sample = float;
using SamplePosition = uint64_t;

struct MidiEvent
{
  uint8_t raw[3] { 0x0 };
  std::chrono::high_resolution_clock::time_point timestamp;
};

struct SampleFrame
{
  Sample left;
  Sample right;
};

struct __attribute__((packed)) SampleInt24
{
  uint8_t a;
  uint8_t b;
  uint8_t c;
};

using SampleInt16 = int16_t;
using SampleInt32 = int32_t;
using SampleFloat32 = float;
