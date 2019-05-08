#pragma once

#include "playground.h"

class DelayedJob
{
 private:
 public:
  typedef function<void()> tJob;

  DelayedJob(int maxDelayMS, tJob job);
  virtual ~DelayedJob();

  void trigger();

 private:
  DelayedJob(const DelayedJob& other) = delete;
  DelayedJob& operator=(const DelayedJob&) = delete;

  bool onTimeout();

  int m_maxDelay;
  tJob m_job;
  sigc::connection m_timeout;
};
