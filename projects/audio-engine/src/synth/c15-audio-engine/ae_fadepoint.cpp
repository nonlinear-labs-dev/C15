#include "ae_fadepoint.h"
#include <nltools/logging/Log.h>
#include <thread>

/******************************************************************************/
/** @file       ae_fadepoint.cpp
    @date
    @version    1.7-4
    @author     M. Seeber
    @brief      fadepoint generator
    @todo
*******************************************************************************/

#include <cmath>

atomic_fade_table::atomic_fade_table()
{
}

void atomic_fade_table::init(const float _samplerate)
{
  const float fade_time = 1e-3f * C15::Config::fade_time_ms * _samplerate;
  const float fade_dx = 1.0f / (fade_time - 1.0f);
  const uint32_t data_size = static_cast<uint32_t>(fade_time);
  m_currentMuteRampIndex = m_finalMuteRampIndex = data_size - 1;
  m_data.resize(data_size);
  for(uint32_t i = 0; i < data_size; i++)
  {
    const float fade_value = std::sin(1.5707963f * static_cast<float>(i) * fade_dx);
    m_data[i] = fade_value * fade_value;
  }
}

void atomic_fade_table::evalTaskStatus()
{
  const uint32_t targetRampIndex = m_shouldMute ? 0 : m_finalMuteRampIndex;
  if(m_currentMuteRampIndex != targetRampIndex)
  {
    if(m_currentMuteRampIndex > targetRampIndex)
    {
      // fade out
      m_currentMuteRampIndex--;
    }
    else
    {
      // fade in
      m_currentMuteRampIndex++;
    }
  }
}

float atomic_fade_table::getValue()
{
  return m_data[m_currentMuteRampIndex];
}

bool atomic_fade_table::isMuted() const
{
  return m_currentMuteRampIndex == 0;
}

void atomic_fade_table::waitForFadeSpinning() const
{
  auto start = std::chrono::high_resolution_clock::now();

  while(m_currentMuteRampIndex)
  {
    std::this_thread::yield();
    auto span = std::chrono::high_resolution_clock::now() - start;
    if(span > std::chrono::seconds(1))
    {
      nltools::Log::warning("Timeout during fade!");
      break;
    }
  }
}
