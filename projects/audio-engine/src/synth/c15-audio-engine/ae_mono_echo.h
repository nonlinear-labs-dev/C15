#pragma once

/******************************************************************************/
/** @file       ae_mono_echo.h
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      ...
    @todo
*******************************************************************************/

#include <vector>
#include "nltoolbox.h"
#include "ae_info.h"

#define ECHO_BUFFER_SIZE 131072

namespace Engine
{
  class MonoEcho
  {
   public:
    float m_out_L = 0.0f, m_out_R = 0.0f;
    MonoEcho();
    void init(const float _samplerate, const uint32_t _upsampleFactor);
    void set(MonoSignals &_signals);
    void apply(MonoSignals &_signals, const float _rawSample_L, const float _rawSample_R);
    void resetDSP();

   private:
    // prewarp
    float m_warpConst_PI = 0.0f;
    float m_freqClip_min = 0.0f, m_freqClip_max = 0.0f;
    // 1p hp
    float m_hp_b0, m_hp_b1 = 0.0f, m_hp_a1 = 0.0f;
    float m_hp_stateVar_L1 = 0.0f, m_hp_stateVar_L2 = 0.0f;
    float m_hp_stateVar_R1 = 0.0f, m_hp_stateVar_R2 = 0.0f;
    // 1p lp
    float m_lp_b0 = 0.0f, m_lp_b1 = 0.0f, m_lp_a1 = 0.0f;
    float m_lp_stateVar_L1 = 0.0f, m_lp_stateVar_L2 = 0.0f;
    float m_lp_stateVar_R1 = 0.0f, m_lp_stateVar_R2 = 0.0f;
    // 2 Hz lp
    float m_lp2hz_b0 = 0.0f;
    float m_lp2hz_stateVar_L = 0.0f, m_lp2hz_stateVar_R = 0.0f;
    // delay buffer
    float m_stateVar_L = 0.0f, m_stateVar_R = 0.0f;
    int32_t m_buffer_indx = 0;
    int32_t m_buffer_sz_m1 = 0;
    std::vector<float> m_buffer_L;
    std::vector<float> m_buffer_R;
  };
}  // namespace Engine
