#include "HighPriorityTask.h"
#include <nltools/logging/Log.h>
#include <mutex>

HighPriorityTask::HighPriorityTask(int bindToCore, std::function<void()> cb)
{
  m_thread = std::thread([=]() {
    setupPrios(bindToCore);
    cb();
  });
}

HighPriorityTask::~HighPriorityTask()
{
  if(m_thread.joinable())
    m_thread.join();
}

void HighPriorityTask::setupPrios(int bindToCore)
{
  static std::mutex m;
  std::lock_guard<std::mutex> g(m);

  prioritizeThread();

  if(bindToCore >= 0)
    setThreadAffinity(bindToCore);
}

void HighPriorityTask::prioritizeThread()
{
  sched_param p;
  auto scheduler = SCHED_FIFO;
  p.sched_priority = sched_get_priority_max(scheduler);
  nltools::Log::info("scheduling priority of", p.sched_priority);

  if(auto r = pthread_setschedparam(pthread_self(), scheduler, &p))
    nltools::Log::warning("Could not set thread priority - consider 'sudo setcap 'cap_sys_nice=eip' <application>'", r);
}

void HighPriorityTask::setThreadAffinity(int bindToCore)
{
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(bindToCore, &set);

  if(sched_setaffinity(0, sizeof(cpu_set_t), &set) < 0)
    nltools::Log::warning("Could not set thread affinity");
}
