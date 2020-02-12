#pragma once

#include "Types.h"
#include <functional>

class MidiOutput
{
 public:
  MidiOutput();
  virtual ~MidiOutput();

  virtual void send(const uint8_t* data, size_t numBytes) = 0;
};
