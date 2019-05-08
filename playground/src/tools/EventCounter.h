#pragma once

#include "playground.h"
#include <array>

class EventCounter : public sigc::trackable
{
 public:
  EventCounter(const char *name);
  virtual ~EventCounter();

  void count();

 private:
  int maintainCurrentSlot();
  bool trace();

  const char *m_name = nullptr;
  uint64_t m_startTime = 0;
  std::array<size_t, 8> m_counters;
  int m_lastSlot = -1;
};
