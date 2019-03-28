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

const AudioOutput::Performance &AudioOutput::getPerformance() const
{
  return m_performance;
}

void AudioOutput::resetPerformance()
{
  m_resetPerformance = true;
}

void AudioOutput::reportPerformanceRatio(double ratio)
{
  if(m_resetPerformance.exchange(false) == true)
    m_performance = {};

  if(m_performance.min == 0)
    m_performance.min = ratio;

  m_performance.min = std::min(m_performance.min, ratio);
  m_performance.avg = std::max(m_performance.avg, m_performance.min);
  m_performance.avg = m_performance.avg * 0.98 + 0.02 * ratio;
  m_performance.max = std::max(m_performance.max, ratio);
}
