#pragma once

#include "midi/AlsaMidiOutput.h"
#include <nltools/threading/Expiration.h>
#include <memory>

class MidiHeartBeat
{
 public:
  MidiHeartBeat(const std::string &device);

 private:
  void send();

  std::unique_ptr<MidiOutput> m_output;
  Expiration m_expiration;
  uint64_t m_counter = 0;
};
