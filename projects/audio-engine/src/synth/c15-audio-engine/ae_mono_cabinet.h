#pragma once

/******************************************************************************/
/** @file       ae_mono_cabinet.h
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      ...
    @todo
*******************************************************************************/

#include "nltoolbox.h"
#include "ae_info.h"

namespace Engine
{
  class MonoCabinet
  {
   public:
    float m_out_L = 0.0f, m_out_R = 0.0f;
    MonoCabinet();
    void init(const float _samplerate);
    void set(MonoSignals &_signals);
    void apply(MonoSignals &_signals, const float _rawSample_L, const float _rawSample_R);
    void resetDSP();

   private:
    // prewarp
    float m_warpConst_2PI = 0.0f;
    float m_freqClip_max = 0.0f, m_freqClip_min = 0.0f;
    // biquad hp
    float m_hp_b0 = 0.0f, m_hp_b1 = 0.0f;
    float m_hp_a1 = 0.0f, m_hp_a2 = 0.0f;
    float m_hp_stateVar_L1 = 0.0f, m_hp_stateVar_R1 = 0.0f;
    float m_hp_stateVar_L2 = 0.0f, m_hp_stateVar_R2 = 0.0f;
    float m_hp_stateVar_L3 = 0.0f, m_hp_stateVar_R3 = 0.0f;
    float m_hp_stateVar_L4 = 0.0f, m_hp_stateVar_R4 = 0.0f;
    // biquad lp
    float m_lp1_b0 = 0.0f, m_lp1_b1 = 0.0f;
    float m_lp1_a1 = 0.0f, m_lp1_a2 = 0.0f;
    float m_lp1_stateVar_L1 = 0.0f, m_lp1_stateVar_R1 = 0.0f;
    float m_lp1_stateVar_L2 = 0.0f, m_lp1_stateVar_R2 = 0.0f;
    float m_lp1_stateVar_L3 = 0.0f, m_lp1_stateVar_R3 = 0.0f;
    float m_lp1_stateVar_L4 = 0.0f, m_lp1_stateVar_R4 = 0.0f;
    float m_lp2_b0 = 0.0f, m_lp2_b1 = 0.0f;
    float m_lp2_a1 = 0.0f, m_lp2_a2 = 0.0f;
    float m_lp2_stateVar_L1 = 0.0f, m_lp2_stateVar_R1 = 0.0f;
    float m_lp2_stateVar_L2 = 0.0f, m_lp2_stateVar_R2 = 0.0f;
    float m_lp2_stateVar_L3 = 0.0f, m_lp2_stateVar_R3 = 0.0f;
    float m_lp2_stateVar_L4 = 0.0f, m_lp2_stateVar_R4 = 0.0f;
    // tilt low shelves
    float m_tiltOmegaSin = 0.0f, m_tiltOmegaCos = 0.0f;
    float m_ls1_b0 = 0.0f, m_ls1_b1 = 0.0f, m_ls1_b2 = 0.0f;
    float m_ls1_a1 = 0.0f, m_ls1_a2 = 0.0f;
    float m_ls1_stateVar_L1 = 0.0f, m_ls1_stateVar_R1 = 0.0f;
    float m_ls1_StateVar_L2 = 0.0f, m_ls1_stateVar_R2 = 0.0f;
    float m_ls1_StateVar_L3 = 0.0f, m_ls1_stateVar_R3 = 0.0f;
    float m_ls1_StateVar_L4 = 0.0f, m_ls1_stateVar_R4 = 0.0f;
    float m_ls2_b0 = 0.0f, m_ls2_b1 = 0.0f, m_ls2_b2 = 0.0f;
    float m_ls2_a1 = 0.0f, m_ls2_a2 = 0.0f;
    float m_ls2_stateVar_L1 = 0.0f, m_ls2_stateVar_R1 = 0.0f;
    float m_ls2_StateVar_L2 = 0.0f, m_ls2_stateVar_R2 = 0.0f;
    float m_ls2_StateVar_L3 = 0.0f, m_ls2_stateVar_R3 = 0.0f;
    float m_ls2_StateVar_L4 = 0.0f, m_ls2_stateVar_R4 = 0.0f;
    // 30 Hz hp
    float m_hp30_b0 = 0.0f;
    float m_hp30_stateVar_L = 0.0f, m_hp30_stateVar_R = 0.0f;
  };
}  // namespace Engine
