#pragma once

#include "MidiOutput.h"
#include <giomm.h>
#include <memory>

class AlsaMidiOutput : public MidiOutput
{
 public:
  AlsaMidiOutput(const std::string& device);
  ~AlsaMidiOutput();

  void send(const uint8_t* data, size_t numBytes) override;

 private:
  snd_rawmidi_t* m_handle = nullptr;
};
