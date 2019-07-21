#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <asoundlib.h>
#include <iostream>
#include <chrono>

using Sample = float;
using SamplePosition = uint64_t;

struct MidiEvent
{
  uint8_t raw[3];
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

static const auto c_numVoices = 1024;

#define TRACE(fn, ln) std::cerr << fn << ": " << ln << std::endl
#define TRACE_HERE() TRACE(__PRETTY_FUNCTION__, __LINE__)
