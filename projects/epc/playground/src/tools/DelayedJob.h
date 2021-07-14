#pragma once

#include "playground.h"
#include <functional>
#include <sigc++/connection.h>

class DelayedJob
{
 private:
 public:
  typedef std::function<void()> tJob;

  DelayedJob(int maxDelayMS, tJob job);
  DelayedJob(const DelayedJob& other) = delete;
  DelayedJob& operator=(const DelayedJob&) = delete;
  virtual ~DelayedJob();

  void trigger();

 private:
  bool onTimeout();

  int m_maxDelay;
  tJob m_job;
  sigc::connection m_timeout;
};
