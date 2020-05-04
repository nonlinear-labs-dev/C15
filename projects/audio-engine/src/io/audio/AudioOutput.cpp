#include "AudioOutput.h"
#include <math.h>

AudioOutput::AudioOutput()
{
}

void AudioOutput::start()
{
}

void AudioOutput::stop()
{
}

AudioOutput::Performance AudioOutput::exhaustPerformance()
{
  auto ret = m_performance;
  m_exhaustPerformance = true;
  return ret;
}

void AudioOutput::resetPerformance()
{
  m_resetPerformance = true;
}

void AudioOutput::reportPerformanceRatio(double ratio)
{
  if(m_exhaustPerformance.exchange(false) == true)
  {
    m_performance.max = ratio;
    m_performance.min = ratio;
    m_performance.sum = 0;
    m_performance.num = 0;
  }

  if(m_resetPerformance.exchange(false) == true)
  {
    m_performance.overallMax = ratio;
    m_performance.overallMin = ratio;
    m_performance.max = ratio;
    m_performance.min = ratio;
    m_performance.sum = 0;
    m_performance.num = 0;
  }

  m_performance.sum += ratio;
  m_performance.num++;

  m_performance.min = std::min(m_performance.min, ratio);
  m_performance.max = std::max(m_performance.max, ratio);

  m_performance.overallMin = std::min(m_performance.overallMin, ratio);
  m_performance.overallMax = std::max(m_performance.overallMax, ratio);
}
