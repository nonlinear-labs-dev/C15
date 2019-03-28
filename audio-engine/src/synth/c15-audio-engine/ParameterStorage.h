#pragma once

#include "pe_defines_config.h"
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
      p->m_paramsignaldataV[voice][param] = f;
      p->m_paramsignaldataP[param][voice] = f;
      return f;
    }

    ParameterStorage *p;
    float &f;
    uint32_t voice;
    uint32_t param;
  };

  inline float operator[](uint32_t param) const
  {
    return m_boundVoice[param];
  }

  inline Accessor operator[](uint32_t param)
  {
    return Accessor(this, m_boundVoice[param], m_voice, param);
  }

  inline ParameterStorage &bindToVoice(uint32_t v)
  {
    m_voice = v;
    m_boundVoice = m_paramsignaldataV[v];
    return *this;
  }

  inline float get(uint32_t voice, uint32_t param)
  {
    return m_paramsignaldataV[voice][param];
  }

 private:
  float m_paramsignaldataV[dsp_number_of_voices][sig_number_of_signal_items] = {};
  float m_paramsignaldataP[sig_number_of_signal_items][dsp_number_of_voices] = {};
  float *m_boundVoice = m_paramsignaldataV[0];
  uint32_t m_voice = 0;
};
