#pragma once

#include <atomic>
#include <stdlib.h>

class AudioOutput
{
 public:
  AudioOutput();

  struct Performance
  {
    double overallMin = 0;
    double min = 0;
    double sum = 0;
    size_t num = 0;
    double max = 0;
    double overallMax = 0;
  };

  virtual void start();
  virtual void stop();

  Performance exhaustPerformance();
  void resetPerformance();

 protected:
  void reportPerformanceRatio(double ratio);

 private:
  Performance m_performance;
  std::atomic<bool> m_resetPerformance = true;
  std::atomic<bool> m_exhaustPerformance = true;
};
