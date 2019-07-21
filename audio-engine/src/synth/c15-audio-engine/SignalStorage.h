#pragma once

#include "pe_defines_config.h"
#include "dsp_defines_signallabels.h"
#include "SignalInfo.h"
#include "ParallelData.h"
#include <stdlib.h>
#include <type_traits>

class SignalStorage
{
 public:
  SignalStorage() = default;

  template <Signals s> inline std::enable_if_t<SignalInfo::MonoSignals::Contains<s>::value, const float &> get() const
  {
    return m_paramsignaldataM[SignalInfo::MonoSignals::IndexMapper<s>::value];
  }

  template <Signals s>
  inline std::enable_if_t<SignalInfo::PolySignals::Contains<s>::value, const FloatVector &> get() const
  {
    return m_paramsignaldataP[SignalInfo::PolySignals::IndexMapper<s>::value];
  }

  template <Signals s> inline void set(std::enable_if_t<SignalInfo::MonoSignals::Contains<s>::value, float> value)
  {
    m_paramsignaldataM[SignalInfo::MonoSignals::IndexMapper<s>::value] = value;
  }

  template <Signals s>
  inline std::enable_if_t<SignalInfo::PolySignals::Contains<s>::value, void> set(uint32_t voice, float value)
  {
    m_paramsignaldataP[SignalInfo::PolySignals::IndexMapper<s>::value][voice] = value;
  }

  inline const float &getOld(Signals s) const
  {
    return m_paramsignaldataM[SignalInfo::MonoSignals::mapToIndex(s)];
  }

  inline const float &getOld(uint32_t voice, Signals s) const
  {
    return m_paramsignaldataP[SignalInfo::PolySignals::mapToIndex(s)][voice];
  }

  inline void setOld(Signals s, float value)
  {
    m_paramsignaldataM[SignalInfo::MonoSignals::mapToIndex(s)] = value;
  }

  inline void setOld(Signals s, uint32_t voice, float value)
  {
    m_paramsignaldataP[SignalInfo::PolySignals::mapToIndex(s)][voice] = value;
  }

 private:
  FloatVector m_paramsignaldataP[SignalInfo::PolySignals::size] = {};
  float m_paramsignaldataM[SignalInfo::MonoSignals::size] = {};
};
