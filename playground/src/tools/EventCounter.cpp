#include "EventCounter.h"
#include <device-settings/DebugLevel.h>
#include <glib.h>

EventCounter::EventCounter(const char *name)
    : m_name(name)
{
  Glib::MainContext::get_default()->signal_timeout().connect_seconds(sigc::mem_fun(this, &EventCounter::trace), 1);
  std::fill(m_counters.begin(), m_counters.end(), 0);
}

EventCounter::~EventCounter()
{
}

int EventCounter::maintainCurrentSlot()
{
  uint64_t now = g_get_monotonic_time();

  if(m_startTime == 0)
    m_startTime = now;

  auto slot = ((now - m_startTime) / 1000000) % m_counters.size();

  if(slot != m_lastSlot)
  {
    m_lastSlot = slot;
    m_counters[slot] = 0;
  }

  return slot;
}

void EventCounter::count()
{
  auto slot = maintainCurrentSlot();
  m_counters[slot]++;
}

bool EventCounter::trace()
{
  auto currentSlot = maintainCurrentSlot();
  std::stringstream str;
  str << "EventCounter: " << m_name << ": ";

  for(int i = 1; i < m_counters.size(); i++)
  {
    auto slot = (currentSlot - i);

    if(slot < 0)
      slot = m_counters.size() + slot;

    str << m_counters[slot];

    if(i < (m_counters.size() - 1))
      str << " / ";
  }

  DebugLevel::warning(str.str());

  return true;
}
