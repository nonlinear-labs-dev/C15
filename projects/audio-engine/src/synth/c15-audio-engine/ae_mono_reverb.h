#pragma once

/******************************************************************************/
/** @file       ae_mono_reverb.h
    @date
    @version    1.7-0
    @author     A.Schmied, M. Seeber
    @brief      ...
    @todo
*******************************************************************************/

#include <vector>
#include "nltoolbox.h"
#include "ae_info.h"

#define REVERB_BUFFER_SIZE 16384

#define REV_G_1 0.617748f
#define REV_G_2 0.630809f
#define REV_G_3 0.64093f
#define REV_G_4 0.653011f

/*********** Primes - 1 for 48 kHz ***********/
#define REV_DEL_L1 280   // 563
#define REV_DEL_L2 1122  // 2243
#define REV_DEL_L3 862   // 1723
#define REV_DEL_L4 466   // 937
#define REV_DEL_L5 718   // 1439
#define REV_DEL_L6 1030  // 2063
#define REV_DEL_L7 886   // 1777
#define REV_DEL_L8 1216  // 2437
#define REV_DEL_R1 378   // 757
#define REV_DEL_R2 1102  // 2003
#define REV_DEL_R3 928   // 1861
#define REV_DEL_R4 490   // 577
#define REV_DEL_R5 682   // 1361
#define REV_DEL_R6 1018  // 2039
#define REV_DEL_R7 858   // 1721
#define REV_DEL_R8 1366  // 1733

#define REV_DEL_L9 2916  // 5827
#define REV_DEL_R9 2676  // 5351

namespace Engine
{
  class MonoReverb
  {
   public:
    MonoReverb();
    float m_out_L = 0.0f, m_out_R = 0.0f, m_out_dry = 0.0f, m_out_wet = 0.0f;

    void init(const float _samplerate, const uint32_t _upsampleFactor);
    void set(MonoSignals &_signals);
    void apply(MonoSignals &_signals, const float _rawSample_L, const float _rawSample_R);
    void resetDSP();

   private:
    uint32_t m_slow_tick = 0;
    uint32_t m_slow_thrsh = 0;

    //************************** Reverb Modulation ***************************//
    float m_mod_1a = 0.0f, m_mod_2a = 0.0f, m_mod_1b = 0.0f, m_mod_2b = 0.0f;
    float m_lfo_omega_1 = 0.0f, m_lfo_omega_2 = 0.0f;
    float m_lfo_stateVar_1 = 0.0f, m_lfo_stateVar_2 = 0.0f;

    //****************************** Loop Filter *****************************//
    float m_warpConst_PI = 0.0f;
    float m_omegaClip_max = 0.0f;

    float m_lp_a0 = 0.0f, m_lp_a1 = 0.0f, m_lp_omega = 0.0f;
    float m_hp_a0 = 0.0f, m_hp_a1 = 0.0f, m_hp_omega = 0.0f;

    float m_lp_stateVar_L = 0.0f, m_lp_stateVar_R = 0.0f;
    float m_hp_stateVar_L = 0.0f, m_hp_stateVar_R = 0.0f;

    //***************************** Delay Buffer *****************************//
    float m_fb_amnt = 0.0f;
    float m_absorb = 0.0f;
    float m_bal_half = 0.0f, m_bal_full = 0.0f;

    uint32_t m_buffer_indx = 0;
    uint32_t m_buffer_sz_m1 = 0, m_buffer_sz_m2 = 0;

    float m_preDel_L = 0.0f, m_preDel_R = 0.0f;

    std::vector<float> m_buffer_L;
    std::vector<float> m_buffer_R;

    std::vector<float> m_buffer_L1;
    std::vector<float> m_buffer_L2;
    std::vector<float> m_buffer_L3;
    std::vector<float> m_buffer_L4;
    std::vector<float> m_buffer_L5;
    std::vector<float> m_buffer_L6;
    std::vector<float> m_buffer_L7;
    std::vector<float> m_buffer_L8;
    std::vector<float> m_buffer_L9;

    std::vector<float> m_buffer_R1;
    std::vector<float> m_buffer_R2;
    std::vector<float> m_buffer_R3;
    std::vector<float> m_buffer_R4;
    std::vector<float> m_buffer_R5;
    std::vector<float> m_buffer_R6;
    std::vector<float> m_buffer_R7;
    std::vector<float> m_buffer_R8;
    std::vector<float> m_buffer_R9;

    float m_stateVar_L1 = 0.0f, m_stateVar_R1 = 0.0f;
    float m_stateVar_L2 = 0.0f, m_stateVar_R2 = 0.0f;
    float m_stateVar_L3 = 0.0f, m_stateVar_R3 = 0.0f;
    float m_stateVar_L4 = 0.0f, m_stateVar_R4 = 0.0f;
    float m_stateVar_L5 = 0.0f, m_stateVar_R5 = 0.0f;
    float m_stateVar_L6 = 0.0f, m_stateVar_R6 = 0.0f;
    float m_stateVar_L7 = 0.0f, m_stateVar_R7 = 0.0f;
    float m_stateVar_L8 = 0.0f, m_stateVar_R8 = 0.0f;
    float m_stateVar_L9 = 0.0f, m_stateVar_R9 = 0.0f;

    //******************************* Smoothing ******************************//
    float m_smooth_inc = 0.0f;

    float m_depth = 0.0f;
    float m_depth_ramp = 0.0f;
    float m_depth_target = 0.0f;
    float m_depth_base = 0.0f;
    float m_depth_diff = 0.0f;

    float m_size = 0.0f;
    float m_size_ramp = 0.0f;
    float m_size_target = 0.0f;
    float m_size_base = 0.0f;
    float m_size_diff = 0.0f;

    float m_bal = 0.0f;
    float m_bal_ramp = 0.0f;
    float m_bal_target = 0.0f;
    float m_bal_base = 0.0f;
    float m_bal_diff = 0.0f;

    float m_preDel_L_ramp = 0.0f;
    float m_preDel_L_target = 0.0f;
    float m_preDel_L_base = 0.0f;
    float m_preDel_L_diff = 0.0f;

    float m_preDel_R_ramp = 0.0f;
    float m_preDel_R_target = 0.0f;
    float m_preDel_R_base = 0.0f;
    float m_preDel_R_diff = 0.0f;

    float m_lp_omega_ramp = 0.0f;
    float m_lp_omega_target = 0.0f;
    float m_lp_omega_base = 0.0f;
    float m_lp_omega_diff = 0.0f;

    float m_hp_omega_ramp = 0.0f;
    float m_hp_omega_target = 0.0f;
    float m_hp_omega_base = 0.0f;
    float m_hp_omega_diff = 0.0f;
  };
}
