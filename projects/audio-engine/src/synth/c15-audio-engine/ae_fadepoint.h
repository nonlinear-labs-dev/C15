#pragma once

/******************************************************************************/
/** @file       ae_fadepoint.h
    @date
    @version    1.7-4
    @author     M. Seeber
    @brief      fadepoint generator
    @todo
*******************************************************************************/

#include <c15_config.h>
#include <vector>

class atomic_fade_table
{
 public:
  atomic_fade_table();
  void init(const float _samplerate);
  void evalTaskStatus();

  template <typename Callable> void muteAndDo(Callable &&c)
  {
    m_shouldMute = true;
    m_numMuteSampleRendered = 0;
    waitForFadeSpinning();
    c();
    m_shouldMute = false;
  }

  float getValue();
  bool isMuted() const;

  void waitForFadeSpinning() const;

  void onMuteSampleRendered()
  {
    m_numMuteSampleRendered++;
  }

  uint32_t getNumMuteSamplesRendered() const
  {
    return m_numMuteSampleRendered;
  }

 private:
  uint32_t m_currentMuteRampIndex = 0, m_finalMuteRampIndex = 0;
  uint32_t m_numMuteSampleRendered = 0;
  std::vector<float> m_data;
  bool m_shouldMute = false;
};
