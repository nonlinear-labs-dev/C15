#pragma once

#include <thread>
#include <functional>

class HighPriorityTask
{
 public:
  HighPriorityTask(int bindToCore, std::function<void()> cb);
  virtual ~HighPriorityTask();

 private:
  void setupPrios(int bindToCore);
  void prioritizeThread();
  void setThreadAffinity(int bindToCore);

  std::thread m_thread;
};
