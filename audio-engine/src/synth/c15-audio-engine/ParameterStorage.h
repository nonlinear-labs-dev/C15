#pragma once

#include "pe_defines_config.h"
#include "ParallelData.h"
#include <stdlib.h>

class ParameterStorage
{
 public:
  ParameterStorage() = default;

  struct Accessor
  {
    inline Accessor(ParameterStorage *p, float &f, uint32_t voice, uint32_t param)
        : p(p)
        , f(f)
        , voice(voice)
        , param(param)
    {
    }

    inline operator const float() const
    {
      return f;
    }

    inline float operator=(float f)
    {
      p->m_paramsignaldataP[param][voice] = f;
      return f;
    }

    ParameterStorage *p;
    float &f;
    uint32_t voice;
    uint32_t param;
  };

  FloatVector &getParameterForAllVoices(uint32_t param)
  {
    return m_paramsignaldataP[param];
  }

  const FloatVector &getParameterForAllVoices(uint32_t param) const
  {
    return m_paramsignaldataP[param];
  }

  inline float operator[](uint32_t param) const
  {
    return get(m_voice, param);
  }

  inline Accessor operator[](uint32_t param)
  {
    return Accessor(this, m_paramsignaldataP[param][m_voice], m_voice, param);
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
