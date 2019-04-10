#pragma once

#include "pe_defines_config.h"
#include "ParallelData.h"
#include <stdlib.h>

class ParameterStorage
{
 public:
  ParameterStorage() = default;

  FloatVector &getParameterForAllVoices(uint32_t param)
  {
    return m_paramsignaldataP[param];
  }

  const FloatVector &getParameterForAllVoices(uint32_t param) const
  {
    return m_paramsignaldataP[param];
  }

  inline const float &operator[](uint32_t param) const
  {
    return m_paramsignaldataP[param][m_voice];
  }

  inline float &operator[](uint32_t param)
  {
    return m_paramsignaldataP[param][m_voice];
  }

  inline ParameterStorage &bindToVoice(uint32_t v)
  {
    m_voice = v;
    return *this;
  }

  inline float get(uint32_t voice, uint32_t param) const
  {
    return m_paramsignaldataP[param][voice];
  }

 private:
  FloatVector m_paramsignaldataP[sig_number_of_signal_items] = {};
  uint32_t m_voice = 0;
};
