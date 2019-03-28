#pragma once

#include <atomic>

class AudioOutput
{
 public:
  AudioOutput();

  struct Performance
  {
    double min = 0;
    double avg = 0;
    double max = 0;
  };

  virtual void start();
  virtual void stop();

  const Performance &getPerformance() const;
  void resetPerformance();

 protected:
  void reportPerformanceRatio(double ratio);

 private:
  Performance m_performance;
  std::atomic<bool> m_resetPerformance;
};
