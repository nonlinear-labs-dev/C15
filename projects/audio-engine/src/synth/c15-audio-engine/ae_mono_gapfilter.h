#pragma once

/******************************************************************************/
/** @file       ae_mono_gapfilter.h
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
  class MonoGapFilter
  {
   public:
    float m_out_L = 0.0f, m_out_R = 0.0f;
    MonoGapFilter();
    void init(const float _samplerate);
    void set(MonoSignals &_signals);
    void apply(MonoSignals &_signals, const float _rawSample_L, const float _rawSample_R);
    void resetDSP();

   private:
    // prewarp
    float m_warpConst_2PI = 0.0f, m_freqScale_norm = 0.0f;
    float m_freqClip_max = 0.0f, m_freqClip_min = 0.0f;
    float m_freqScale_max = 0.0f, m_freqScale_min = 0.0f;
    // biquad hp l/r
    float m_hp_l1_b0 = 0.0f, m_hp_l1_b1 = 0.0f, m_hp_l2_b0 = 0.0f, m_hp_l2_b1 = 0.0f;
    float m_hp_l1_a1 = 0.0f, m_hp_l1_a2 = 0.0f, m_hp_l2_a1 = 0.0f, m_hp_l2_a2 = 0.0f;
    float m_hp_l1_stateVar_1 = 0.0f, m_hp_l2_stateVar_1 = 0.0f;
    float m_hp_l1_stateVar_2 = 0.0f, m_hp_l2_stateVar_2 = 0.0f;
    float m_hp_l1_stateVar_3 = 0.0f, m_hp_l2_stateVar_3 = 0.0f;
    float m_hp_l1_stateVar_4 = 0.0f, m_hp_l2_stateVar_4 = 0.0f;
    float m_hp_r1_b0 = 0.0f, m_hp_r1_b1 = 0.0f, m_hp_r2_b0 = 0.0f, m_hp_r2_b1 = 0.0f;
    float m_hp_r1_a1 = 0.0f, m_hp_r1_a2 = 0.0f, m_hp_r2_a1 = 0.0f, m_hp_r2_a2 = 0.0f;
    float m_hp_r1_stateVar_1 = 0.0f, m_hp_r2_stateVar_1 = 0.0f;
    float m_hp_r1_stateVar_2 = 0.0f, m_hp_r2_stateVar_2 = 0.0f;
    float m_hp_r1_stateVar_3 = 0.0f, m_hp_r2_stateVar_3 = 0.0f;
    float m_hp_r1_stateVar_4 = 0.0f, m_hp_r2_stateVar_4 = 0.0f;
    // biquad lp l/r
    float m_lp_l1_b0 = 0.0f, m_lp_l1_b1 = 0.0f, m_lp_l2_b0 = 0.0f, m_lp_l2_b1 = 0.0f;
    float m_lp_l1_a1 = 0.0f, m_lp_l1_a2 = 0.0f, m_lp_l2_a1 = 0.0f, m_lp_l2_a2 = 0.0f;
    float m_lp_l1_stateVar_1 = 0.0f, m_lp_l2_stateVar_1 = 0.0f;
    float m_lp_l1_stateVar_2 = 0.0f, m_lp_l2_stateVar_2 = 0.0f;
    float m_lp_l1_stateVar_3 = 0.0f, m_lp_l2_stateVar_3 = 0.0f;
    float m_lp_l1_stateVar_4 = 0.0f, m_lp_l2_stateVar_4 = 0.0f;
    float m_lp_r1_b0 = 0.0f, m_lp_r1_b1 = 0.0f, m_lp_r2_b0 = 0.0f, m_lp_r2_b1 = 0.0f;
    float m_lp_r1_a1 = 0.0f, m_lp_r1_a2 = 0.0f, m_lp_r2_a1 = 0.0f, m_lp_r2_a2 = 0.0f;
    float m_lp_r1_stateVar_1 = 0.0f, m_lp_r2_stateVar_1 = 0.0f;
    float m_lp_r1_stateVar_2 = 0.0f, m_lp_r2_stateVar_2 = 0.0f;
    float m_lp_r1_stateVar_3 = 0.0f, m_lp_r2_stateVar_3 = 0.0f;
    float m_lp_r1_stateVar_4 = 0.0f, m_lp_r2_stateVar_4 = 0.0f;
  };
}  // namespace Engine
