#pragma once

#include "midi/AlsaMidiOutput.h"
#include <memory>

class MidiHeartBeat
{
 public:
  MidiHeartBeat(const std::string &device);
  ~MidiHeartBeat();

 private:
  bool send();

  std::unique_ptr<MidiOutput> m_output;
  uint64_t m_counter = 0;
  sigc::connection m_timerConnection;
};
