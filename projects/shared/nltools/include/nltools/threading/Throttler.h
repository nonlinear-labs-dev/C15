#pragma once

#include <nltools/threading/Expiration.h>

class Throttler
{
 public:
  using Task = std::function<void()>;

  Throttler(Expiration::Duration maxDelay);

  void doTask(Task&& task);
  bool isPending() const;

  void doActionSync();

 private:
  void delayedCallback();

  Expiration m_expiration;
  Expiration::Duration m_maxDelay;
  Task m_task;
  bool m_isPending = false;
};
