#include "ae_mono_reverb.h"

/******************************************************************************/
/** @file       ae_mono_reverb.cpp
    @date
    @version    1.7-0
    @author     A.Schmied, M. Seeber
    @brief      ...
    @todo
*******************************************************************************/

Engine::MonoReverb::MonoReverb()
{
}

void Engine::MonoReverb::init(const float _samplerate, const uint32_t _upsampleFactor)
{
  m_out_L = 0.0f;
  m_out_R = 0.0f;
  m_out_dry = 0.0f;
  m_out_wet = 0.0f;

  m_slow_tick = 0;
  m_slow_thrsh = 2 * _upsampleFactor - 1;

  //************************** Reverb Modulation ***************************//
  m_mod_1a = 0.0f;
  m_mod_2a = 0.0f;
  m_mod_1b = 0.0f;
  m_mod_2b = 0.0f;
  m_lfo_omega_1 = 0.86306f * 2.0f / _samplerate;
  m_lfo_omega_2 = 0.6666f * 2.0f / _samplerate;

  //****************************** Loop Filter *****************************//
  m_warpConst_PI = NlToolbox::Constants::pi / _samplerate;
  m_omegaClip_max = _samplerate / 2.0f;

  m_lp_stateVar_L = 0.0f;
  m_lp_stateVar_R = 0.0f;
  m_hp_stateVar_L = 0.0f;
  m_hp_stateVar_R = 0.0f;

  //***************************** Delay Buffer *****************************//
  m_buffer_indx = 0;

  m_buffer_sz_m1 = REVERB_BUFFER_SIZE * _upsampleFactor - 1;
  m_buffer_sz_m2 = REVERB_BUFFER_SIZE * _upsampleFactor - 2;

  m_buffer_L.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L1.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L2.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L3.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L4.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L5.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L6.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L7.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L8.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_L9.resize(REVERB_BUFFER_SIZE * _upsampleFactor);

  m_buffer_R.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R1.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R2.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R3.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R4.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R5.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R6.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R7.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R8.resize(REVERB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_R9.resize(REVERB_BUFFER_SIZE * _upsampleFactor);

  std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.0f);
  std::fill(m_buffer_L1.begin(), m_buffer_L1.end(), 0.0f);
  std::fill(m_buffer_L2.begin(), m_buffer_L2.end(), 0.0f);
  std::fill(m_buffer_L3.begin(), m_buffer_L3.end(), 0.0f);
  std::fill(m_buffer_L4.begin(), m_buffer_L4.end(), 0.0f);
  std::fill(m_buffer_L5.begin(), m_buffer_L5.end(), 0.0f);
  std::fill(m_buffer_L6.begin(), m_buffer_L6.end(), 0.0f);
  std::fill(m_buffer_L7.begin(), m_buffer_L7.end(), 0.0f);
  std::fill(m_buffer_L8.begin(), m_buffer_L8.end(), 0.0f);
  std::fill(m_buffer_L9.begin(), m_buffer_L9.end(), 0.0f);

  std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.0f);
  std::fill(m_buffer_R1.begin(), m_buffer_R1.end(), 0.0f);
  std::fill(m_buffer_R2.begin(), m_buffer_R2.end(), 0.0f);
  std::fill(m_buffer_R3.begin(), m_buffer_R3.end(), 0.0f);
  std::fill(m_buffer_R4.begin(), m_buffer_R4.end(), 0.0f);
  std::fill(m_buffer_R5.begin(), m_buffer_R5.end(), 0.0f);
  std::fill(m_buffer_R6.begin(), m_buffer_R6.end(), 0.0f);
  std::fill(m_buffer_R7.begin(), m_buffer_R7.end(), 0.0f);
  std::fill(m_buffer_R8.begin(), m_buffer_R8.end(), 0.0f);
  std::fill(m_buffer_R9.begin(), m_buffer_R9.end(), 0.0f);

  m_stateVar_L1 = 0.0f;
  m_stateVar_L2 = 0.0f;
  m_stateVar_L3 = 0.0f;
  m_stateVar_L4 = 0.0f;
  m_stateVar_L5 = 0.0f;
  m_stateVar_L6 = 0.0f;
  m_stateVar_L7 = 0.0f;
  m_stateVar_L8 = 0.0f;
  m_stateVar_L9 = 0.0f;

  m_stateVar_R1 = 0.0f;
  m_stateVar_R2 = 0.0f;
  m_stateVar_R3 = 0.0f;
  m_stateVar_R4 = 0.0f;
  m_stateVar_R5 = 0.0f;
  m_stateVar_R6 = 0.0f;
  m_stateVar_R7 = 0.0f;
  m_stateVar_R8 = 0.0f;
  m_stateVar_R9 = 0.0f;

  //******************************* Smoothing ******************************//
  m_smooth_inc = 1.0f / std::max(50.0f * (0.001f * _samplerate / 2.0f), 1.e-12f);  /// 50ms als DEfine Bitte!
  // todo: pack settings like this into c15_config.h (for example)
}

void Engine::MonoReverb::set(MonoSignals &_signals)
{
  float tmpVar;
  float tmp_target;
  tmpVar = _signals.get(C15::Signals::Mono_Signals::Reverb_Size);
  tmp_target = _signals.get(C15::Signals::Mono_Signals::Reverb_Chorus) * (tmpVar * -200.0f + 311.0f);
  if(m_depth_target - tmp_target != 0.0f)
  {
    m_depth_target = tmp_target;
    m_depth_base = m_depth;
    m_depth_diff = m_depth_target - m_depth_base;
    m_depth_ramp = 0.0f;
  }

  tmp_target = tmpVar * (0.5f - std::abs(tmpVar) * -0.5f);
  if(m_size_target - tmp_target != 0.0f)
  {
    m_size_target = tmp_target;
    m_size_base = m_size;
    m_size_diff = m_size_target - m_size_base;
    m_size_ramp = 0.0f;
  }

  tmp_target = _signals.get(C15::Signals::Mono_Signals::Reverb_Bal);
  if(m_bal_target - tmp_target != 0.0f)
  {
    m_bal_target = tmp_target;
    m_bal_base = m_bal;
    m_bal_diff = m_bal_target - m_bal_base;
    m_bal_ramp = 0.0f;
  }

  tmpVar = _signals.get(C15::Signals::Mono_Signals::Reverb_Pre);
  tmp_target = std::round(tmpVar);
  if(m_preDel_L_target - tmp_target != 0.0f)
  {
    m_preDel_L_target = tmp_target;
    m_preDel_L_base = m_preDel_L;
    m_preDel_L_diff = m_preDel_L_target - m_preDel_L_base;
    m_preDel_L_ramp = 0.0f;
  }

  tmp_target = std::round(tmpVar * 1.18933f);
  if(m_preDel_R_target - tmp_target != 0.0f)
  {
    m_preDel_R_target = tmp_target;
    m_preDel_R_base = m_preDel_R;
    m_preDel_R_diff = m_preDel_R_target - m_preDel_R_base;
    m_preDel_R_ramp = 0.0f;
  }

  tmp_target = std::clamp(_signals.get(C15::Signals::Mono_Signals::Reverb_LPF), 0.1f, m_omegaClip_max);
  tmp_target = NlToolbox::Math::tan(tmp_target * m_warpConst_PI);
  if(m_lp_omega_target - tmp_target != 0.0f)
  {
    m_lp_omega_target = tmp_target;
    m_lp_omega_base = m_lp_omega;
    m_lp_omega_diff = m_lp_omega_target - m_lp_omega_base;
    m_lp_omega_ramp = 0.0f;
  }

  tmp_target = std::clamp(_signals.get(C15::Signals::Mono_Signals::Reverb_HPF), 0.1f, m_omegaClip_max);
  tmp_target = NlToolbox::Math::tan(tmp_target * m_warpConst_PI);
  if(m_hp_omega_target - tmp_target != 0.0f)
  {
    m_hp_omega_target = tmp_target;
    m_hp_omega_base = m_hp_omega;
    m_hp_omega_diff = m_hp_omega_target - m_hp_omega_base;
    m_hp_omega_ramp = 0.0f;
  }
}

void Engine::MonoReverb::apply(MonoSignals &_signals, const float _rawSample_L, const float _rawSample_R)
{
  float tmpVar;
  int32_t ind_t0, ind_tm1, ind_tp1, ind_tp2;
  float wetSample_L, wetSample_R;

  //************************** Reverb Modulation ***************************//
  if(!m_slow_tick)
  {
    if(m_depth_ramp > 1.0f)  // Depth Smth.
    {
      m_depth = m_depth_target;
    }
    else
    {
      m_depth = m_depth_base + (m_depth_diff * m_depth_ramp);
      m_depth_ramp += m_smooth_inc;
    }

    if(m_size_ramp > 1.0f)  // Size Smth.
    {
      m_size = m_size_target;
    }
    else
    {
      m_size = m_size_base + (m_size_diff * m_size_ramp);
      m_size_ramp += m_smooth_inc;
    }
    m_absorb = m_size * 0.334f + 0.666f;
    m_fb_amnt = m_size * 0.667f + 0.333f;

    if(m_bal_ramp > 1.0f)  // Balance Smth.
    {
      m_bal = m_bal_target;
    }
    else
    {
      m_bal = m_bal_base + (m_bal_diff * m_bal_ramp);
      m_bal_ramp += m_smooth_inc;
    }
    tmpVar = m_bal;
    m_bal_full = tmpVar * (2.0f - tmpVar);
    tmpVar = 1.0f - tmpVar;
    m_bal_half = tmpVar * (2.0f - tmpVar);

    if(m_preDel_L_ramp > 1.0f)  // PreDelay L Smth.
    {
      m_preDel_L = m_preDel_L_target;
    }
    else
    {
      m_preDel_L = m_preDel_L_base + (m_preDel_L_diff * m_preDel_L_ramp);
      m_preDel_L_ramp += m_smooth_inc;
    }

    if(m_preDel_R_ramp > 1.0f)  // PreDelay R Smth.
    {
      m_preDel_R = m_preDel_R_target;
    }
    else
    {
      m_preDel_R = m_preDel_R_base + (m_preDel_R_diff * m_preDel_R_ramp);
      m_preDel_R_ramp += m_smooth_inc;
    }

    if(m_lp_omega_ramp > 1.0f)  // LP Omega Smth.
    {
      m_lp_omega = m_lp_omega_target;
    }
    else
    {
      m_lp_omega = m_lp_omega_base + (m_lp_omega_diff * m_lp_omega_ramp);
      m_lp_omega_ramp += m_smooth_inc;
    }
    m_lp_a0 = 1.0f / (m_lp_omega + 1.0f);
    m_lp_a1 = m_lp_omega - 1.0f;

    if(m_hp_omega_ramp > 1.0f)  // HP Omega Smth.
    {
      m_hp_omega = m_hp_omega_target;
    }
    else
    {
      m_hp_omega = m_hp_omega_base + (m_hp_omega_diff * m_hp_omega_ramp);
      m_hp_omega_ramp += m_smooth_inc;
    }
    m_hp_a0 = 1.0f / (m_hp_omega + 1.0f);
    m_hp_a1 = m_hp_omega - 1.0f;
    tmpVar = m_lfo_stateVar_1 + m_lfo_omega_1;
    tmpVar = tmpVar - std::round(tmpVar);
    m_lfo_stateVar_1 = tmpVar;

    tmpVar = (8.0f - std::abs(tmpVar) * 16.0f) * tmpVar;
    tmpVar += 1.0f;
    m_mod_1a = tmpVar * m_depth;
    m_mod_2a = (1.0f - tmpVar) * m_depth;

    tmpVar = m_lfo_stateVar_2 + m_lfo_omega_2;
    tmpVar = tmpVar - std::round(tmpVar);
    m_lfo_stateVar_2 = tmpVar;

    tmpVar = (8.0f - std::abs(tmpVar) * 16.0f) * tmpVar;
    tmpVar += 1.0f;
    m_mod_1b = tmpVar * m_depth;
    m_mod_2b = (1.0f - tmpVar) * m_depth;
  }

  m_slow_tick = (m_slow_tick + 1) & m_slow_thrsh;
  //************************************************************************//
  //**************************** Left Channel ******************************//
  wetSample_L = _rawSample_L * _signals.get(C15::Signals::Mono_Signals::Reverb_Send)
      * _signals.get(C15::Signals::Mono_Signals::Reverb_Feedback);

  //****************************** Asym 2 L ********************************//
  m_buffer_L[m_buffer_indx] = wetSample_L;

  tmpVar = std::clamp(m_preDel_L, 0.0f, static_cast<float>(m_buffer_sz_m1));  /// this can be in a setter!

  ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
  tmpVar = tmpVar - static_cast<float>(ind_t0);

  ind_tm1 = ind_t0 + 1;

  ind_t0 = m_buffer_indx - ind_t0;
  ind_tm1 = m_buffer_indx - ind_tm1;

  ind_t0 &= m_buffer_sz_m1;
  ind_tm1 &= m_buffer_sz_m1;

  wetSample_L = m_buffer_L[ind_t0] + tmpVar * (m_buffer_L[ind_tm1] - m_buffer_L[ind_t0]);

  wetSample_L += (m_stateVar_R9 * m_fb_amnt);

  //**************************** Loop Filter L *****************************//
  wetSample_L = (wetSample_L - m_lp_stateVar_L * m_lp_a1) * m_lp_a0;
  tmpVar = m_lp_stateVar_L;
  m_lp_stateVar_L = wetSample_L;

  wetSample_L = (wetSample_L + tmpVar) * m_lp_omega;

  wetSample_L = (wetSample_L - m_hp_stateVar_L * m_hp_a1) * m_hp_a0;
  tmpVar = m_hp_stateVar_L;
  m_hp_stateVar_L = wetSample_L;

  wetSample_L = wetSample_L - tmpVar;

  //****************************** Del 4p L1 *******************************//
  tmpVar = m_stateVar_L1 * m_absorb;
  wetSample_L += (tmpVar * REV_G_1);

  m_buffer_L1[m_buffer_indx] = wetSample_L;

  wetSample_L = wetSample_L * -REV_G_1 + tmpVar;

  tmpVar = REV_DEL_L1 + m_mod_2a;
  tmpVar = std::clamp(tmpVar, 1.0f, static_cast<float>(m_buffer_sz_m2));

  ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
  tmpVar = tmpVar - static_cast<float>(ind_t0);

  ind_tm1 = std::max(ind_t0, 1) - 1;
  ind_tp1 = ind_t0 + 1;
  ind_tp2 = ind_t0 + 2;

  ind_tm1 = m_buffer_indx - ind_tm1;
  ind_t0 = m_buffer_indx - ind_t0;
  ind_tp1 = m_buffer_indx - ind_tp1;
  ind_tp2 = m_buffer_indx - ind_tp2;

  ind_tm1 &= m_buffer_sz_m1;
  ind_t0 &= m_buffer_sz_m1;
  ind_tp1 &= m_buffer_sz_m1;
  ind_tp2 &= m_buffer_sz_m1;

  m_stateVar_L1 = NlToolbox::Math::interpolRT(tmpVar, m_buffer_L1[ind_tm1], m_buffer_L1[ind_t0], m_buffer_L1[ind_tp1],
                                              m_buffer_L1[ind_tp2]);

  //***************************** Del 1p L2 ******************************//
  tmpVar = m_stateVar_L2 * m_absorb;
  wetSample_L = wetSample_L + (tmpVar * REV_G_2);

  m_buffer_L2[m_buffer_indx] = wetSample_L;

  wetSample_L = wetSample_L * -REV_G_2 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_L2;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_L2 = m_buffer_L2[ind_t0];

  //***************************** Del 1p L3 ******************************//
  tmpVar = m_stateVar_L3 * m_absorb;
  wetSample_L = wetSample_L + (tmpVar * REV_G_3);

  m_buffer_L3[m_buffer_indx] = wetSample_L;

  wetSample_L = wetSample_L * -REV_G_3 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_L3;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_L3 = m_buffer_L3[ind_t0];

  //***************************** Del 1p L4 ******************************//
  tmpVar = m_stateVar_L4 * m_absorb;
  wetSample_L = wetSample_L + (tmpVar * REV_G_4);

  m_buffer_L4[m_buffer_indx] = wetSample_L;

  float wetSample_L2 = wetSample_L * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_L4;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_L4 = m_buffer_L4[ind_t0];

  //***************************** Del 1p L5 ******************************//
  tmpVar = m_stateVar_L5 * m_absorb;
  wetSample_L = wetSample_L2 + (tmpVar * REV_G_4);

  m_buffer_L5[m_buffer_indx] = wetSample_L;

  wetSample_L = wetSample_L * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_L5;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_L5 = m_buffer_L5[ind_t0];

  //***************************** Del 1p L6 ******************************//
  tmpVar = m_stateVar_L6 * m_absorb;
  wetSample_L = wetSample_L + (tmpVar * REV_G_4);

  m_buffer_L6[m_buffer_indx] = wetSample_L;

  wetSample_L = wetSample_L * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_L6;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_L6 = m_buffer_L6[ind_t0];

  //***************************** Del 1p L7 ******************************//
  tmpVar = m_stateVar_L7 * m_absorb;
  wetSample_L = wetSample_L + (tmpVar * REV_G_4);

  m_buffer_L7[m_buffer_indx] = wetSample_L;

  wetSample_L = wetSample_L * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_L7;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_L7 = m_buffer_L7[ind_t0];

  //***************************** Del 1p L8 ******************************//
  tmpVar = m_stateVar_L8 * m_absorb;
  wetSample_L = wetSample_L + (tmpVar * REV_G_4);

  m_buffer_L8[m_buffer_indx] = wetSample_L;

  wetSample_L = wetSample_L * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_L8;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_L8 = m_buffer_L8[ind_t0];

  //************************************************************************//
  //*************************** Right Channel ******************************//
  wetSample_R = _rawSample_R * _signals.get(C15::Signals::Mono_Signals::Reverb_Send)
      * _signals.get(C15::Signals::Mono_Signals::Reverb_Feedback);

  //****************************** Asym 2 R ********************************//
  m_buffer_R[m_buffer_indx] = wetSample_R;

  tmpVar = std::clamp(m_preDel_R, 0.0f, static_cast<float>(m_buffer_sz_m1));

  ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
  tmpVar = tmpVar - static_cast<float>(ind_t0);

  ind_tm1 = ind_t0 + 1;

  ind_t0 = m_buffer_indx - ind_t0;
  ind_tm1 = m_buffer_indx - ind_tm1;

  ind_t0 &= m_buffer_sz_m1;
  ind_tm1 &= m_buffer_sz_m1;

  wetSample_R = m_buffer_R[ind_t0] + tmpVar * (m_buffer_R[ind_tm1] - m_buffer_R[ind_t0]);

  wetSample_R += (m_stateVar_L9 * m_fb_amnt);

  //**************************** Loop Filter R *****************************//
  wetSample_R = (wetSample_R - m_lp_stateVar_R * m_lp_a1) * m_lp_a0;
  tmpVar = m_lp_stateVar_R;
  m_lp_stateVar_R = wetSample_R;

  wetSample_R = (wetSample_R + tmpVar) * m_lp_omega;

  wetSample_R = (wetSample_R - m_hp_stateVar_R * m_hp_a1) * m_hp_a0;
  tmpVar = m_hp_stateVar_R;
  m_hp_stateVar_R = wetSample_R;

  wetSample_R = wetSample_R - tmpVar;

  //****************************** Del 4p R1 *******************************//
  tmpVar = m_stateVar_R1 * m_absorb;
  wetSample_R += (tmpVar * REV_G_1);

  m_buffer_R1[m_buffer_indx] = wetSample_R;

  wetSample_R = wetSample_R * -REV_G_1 + tmpVar;

  tmpVar = REV_DEL_R1 + m_mod_2b;
  tmpVar = std::clamp(tmpVar, 1.0f, static_cast<float>(m_buffer_sz_m2));

  ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
  tmpVar = tmpVar - static_cast<float>(ind_t0);

  ind_tm1 = std::max(ind_t0, 1) - 1;
  ind_tp1 = ind_t0 + 1;
  ind_tp2 = ind_t0 + 2;

  ind_tm1 = m_buffer_indx - ind_tm1;
  ind_t0 = m_buffer_indx - ind_t0;
  ind_tp1 = m_buffer_indx - ind_tp1;
  ind_tp2 = m_buffer_indx - ind_tp2;

  ind_tm1 &= m_buffer_sz_m1;
  ind_t0 &= m_buffer_sz_m1;
  ind_tp1 &= m_buffer_sz_m1;
  ind_tp2 &= m_buffer_sz_m1;

  m_stateVar_R1 = NlToolbox::Math::interpolRT(tmpVar, m_buffer_R1[ind_tm1], m_buffer_R1[ind_t0], m_buffer_R1[ind_tp1],
                                              m_buffer_R1[ind_tp2]);

  //***************************** Del 1p R2 ******************************//
  tmpVar = m_stateVar_R2 * m_absorb;
  wetSample_R = wetSample_R + (tmpVar * REV_G_2);

  m_buffer_R2[m_buffer_indx] = wetSample_R;

  wetSample_R = wetSample_R * -REV_G_2 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_R2;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_R2 = m_buffer_R2[ind_t0];

  //***************************** Del 1p R3 ******************************//
  tmpVar = m_stateVar_R3 * m_absorb;
  wetSample_R = wetSample_R + (tmpVar * REV_G_3);

  m_buffer_R3[m_buffer_indx] = wetSample_R;

  wetSample_R = wetSample_R * -REV_G_3 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_R3;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_R3 = m_buffer_R3[ind_t0];

  //***************************** Del 1p R4 ******************************//
  tmpVar = m_stateVar_R4 * m_absorb;
  wetSample_R = wetSample_R + (tmpVar * REV_G_4);

  m_buffer_R4[m_buffer_indx] = wetSample_R;

  float wetSample_R2 = wetSample_R * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_R4;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_R4 = m_buffer_R4[ind_t0];

  //***************************** Del 1p R5 ******************************//
  tmpVar = m_stateVar_R5 * m_absorb;
  wetSample_R = wetSample_R2 + (tmpVar * REV_G_4);

  m_buffer_R5[m_buffer_indx] = wetSample_R;

  wetSample_R = wetSample_R * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_R5;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_R5 = m_buffer_R5[ind_t0];

  //***************************** Del 1p R6 ******************************//
  tmpVar = m_stateVar_R6 * m_absorb;
  wetSample_R = wetSample_R + (tmpVar * REV_G_4);

  m_buffer_R6[m_buffer_indx] = wetSample_R;

  wetSample_R = wetSample_R * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_R6;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_R6 = m_buffer_R6[ind_t0];

  //***************************** Del 1p R7 ******************************//
  tmpVar = m_stateVar_R7 * m_absorb;
  wetSample_R = wetSample_R + (tmpVar * REV_G_4);

  m_buffer_R7[m_buffer_indx] = wetSample_R;

  wetSample_R = wetSample_R * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_R7;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_R7 = m_buffer_R7[ind_t0];

  //***************************** Del 1p R8 ******************************//
  tmpVar = m_stateVar_R8 * m_absorb;
  wetSample_R = wetSample_R + (tmpVar * REV_G_4);

  m_buffer_R8[m_buffer_indx] = wetSample_R;

  wetSample_R = wetSample_R * -REV_G_4 + tmpVar;

  ind_t0 = m_buffer_indx - REV_DEL_R8;
  ind_t0 &= m_buffer_sz_m1;

  m_stateVar_R8 = m_buffer_R8[ind_t0];

  //************************************************************************//
  //*************************** Feedback Delay *****************************//
  //****************************** Del 4p L9 *******************************//
  m_buffer_L9[m_buffer_indx] = wetSample_L;

  tmpVar = REV_DEL_L9 + m_mod_1a;
  tmpVar = std::clamp(tmpVar, 0.0f, static_cast<float>(m_buffer_sz_m2));

  ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
  tmpVar = tmpVar - static_cast<float>(ind_t0);

  ind_tm1 = std::max(ind_t0, 1) - 1;
  ind_tp1 = ind_t0 + 1;
  ind_tp2 = ind_t0 + 2;

  ind_tm1 = m_buffer_indx - ind_tm1;
  ind_t0 = m_buffer_indx - ind_t0;
  ind_tp1 = m_buffer_indx - ind_tp1;
  ind_tp2 = m_buffer_indx - ind_tp2;

  ind_tm1 &= m_buffer_sz_m1;
  ind_t0 &= m_buffer_sz_m1;
  ind_tp1 &= m_buffer_sz_m1;
  ind_tp2 &= m_buffer_sz_m1;

  m_stateVar_L9 = NlToolbox::Math::interpolRT(tmpVar, m_buffer_L9[ind_tm1], m_buffer_L9[ind_t0], m_buffer_L9[ind_tp1],
                                              m_buffer_L9[ind_tp2]);

  //***************************** Del 4p R9 ******************************//
  m_buffer_R9[m_buffer_indx] = wetSample_R;

  tmpVar = REV_DEL_R9 + m_mod_1b;
  tmpVar = std::clamp(tmpVar, 0.0f, static_cast<float>(m_buffer_sz_m2));

  ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
  tmpVar = tmpVar - static_cast<float>(ind_t0);

  ind_tm1 = std::max(ind_t0, 1) - 1;
  ind_tp1 = ind_t0 + 1;
  ind_tp2 = ind_t0 + 2;

  ind_tm1 = m_buffer_indx - ind_tm1;
  ind_t0 = m_buffer_indx - ind_t0;
  ind_tp1 = m_buffer_indx - ind_tp1;
  ind_tp2 = m_buffer_indx - ind_tp2;

  ind_tm1 &= m_buffer_sz_m1;
  ind_t0 &= m_buffer_sz_m1;
  ind_tp1 &= m_buffer_sz_m1;
  ind_tp2 &= m_buffer_sz_m1;

  m_stateVar_R9 = NlToolbox::Math::interpolRT(tmpVar, m_buffer_R9[ind_tm1], m_buffer_R9[ind_t0], m_buffer_R9[ind_tp1],
                                              m_buffer_R9[ind_tp2]);

  m_buffer_indx = (m_buffer_indx + 1) & m_buffer_sz_m1;

  //************************************************************************//
  //**************************** Output Mixer ******************************//
  wetSample_L = wetSample_L * m_bal_full + wetSample_L2 * m_bal_half;
  wetSample_R = wetSample_R * m_bal_full + wetSample_R2 * m_bal_half;

  m_out_L = _rawSample_L * _signals.get(C15::Signals::Mono_Signals::Reverb_Dry)
      + wetSample_L * _signals.get(C15::Signals::Mono_Signals::Reverb_Wet);
  m_out_R = _rawSample_R * _signals.get(C15::Signals::Mono_Signals::Reverb_Dry)
      + wetSample_R * _signals.get(C15::Signals::Mono_Signals::Reverb_Wet);

  m_out_dry = _rawSample_L + _rawSample_R;
  m_out_wet = wetSample_L + wetSample_R;
}

void Engine::MonoReverb::resetDSP()
{
  m_out_L = 0.0f;
  m_out_R = 0.0f;
  m_out_dry = 0.0f;
  m_out_wet = 0.0f;

  //****************************** Loop Filter *****************************//

  m_lp_stateVar_L = 0.0f;
  m_lp_stateVar_R = 0.0f;
  m_hp_stateVar_L = 0.0f;
  m_hp_stateVar_R = 0.0f;

  //***************************** Delay Buffer *****************************//

  std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.0f);
  std::fill(m_buffer_L1.begin(), m_buffer_L1.end(), 0.0f);
  std::fill(m_buffer_L2.begin(), m_buffer_L2.end(), 0.0f);
  std::fill(m_buffer_L3.begin(), m_buffer_L3.end(), 0.0f);
  std::fill(m_buffer_L4.begin(), m_buffer_L4.end(), 0.0f);
  std::fill(m_buffer_L5.begin(), m_buffer_L5.end(), 0.0f);
  std::fill(m_buffer_L6.begin(), m_buffer_L6.end(), 0.0f);
  std::fill(m_buffer_L7.begin(), m_buffer_L7.end(), 0.0f);
  std::fill(m_buffer_L8.begin(), m_buffer_L8.end(), 0.0f);
  std::fill(m_buffer_L9.begin(), m_buffer_L9.end(), 0.0f);

  std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.0f);
  std::fill(m_buffer_R1.begin(), m_buffer_R1.end(), 0.0f);
  std::fill(m_buffer_R2.begin(), m_buffer_R2.end(), 0.0f);
  std::fill(m_buffer_R3.begin(), m_buffer_R3.end(), 0.0f);
  std::fill(m_buffer_R4.begin(), m_buffer_R4.end(), 0.0f);
  std::fill(m_buffer_R5.begin(), m_buffer_R5.end(), 0.0f);
  std::fill(m_buffer_R6.begin(), m_buffer_R6.end(), 0.0f);
  std::fill(m_buffer_R7.begin(), m_buffer_R7.end(), 0.0f);
  std::fill(m_buffer_R8.begin(), m_buffer_R8.end(), 0.0f);
  std::fill(m_buffer_R9.begin(), m_buffer_R9.end(), 0.0f);

  m_stateVar_L1 = 0.0f;
  m_stateVar_L2 = 0.0f;
  m_stateVar_L3 = 0.0f;
  m_stateVar_L4 = 0.0f;
  m_stateVar_L5 = 0.0f;
  m_stateVar_L6 = 0.0f;
  m_stateVar_L7 = 0.0f;
  m_stateVar_L8 = 0.0f;
  m_stateVar_L9 = 0.0f;

  m_stateVar_R1 = 0.0f;
  m_stateVar_R2 = 0.0f;
  m_stateVar_R3 = 0.0f;
  m_stateVar_R4 = 0.0f;
  m_stateVar_R5 = 0.0f;
  m_stateVar_R6 = 0.0f;
  m_stateVar_R7 = 0.0f;
  m_stateVar_R8 = 0.0f;
  m_stateVar_R9 = 0.0f;
}
