#include "ae_mono_flanger.h"

/******************************************************************************/
/** @file       ae_mono_flanger.cpp
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      ...
    @todo
*******************************************************************************/

Engine::MonoFlanger::MonoFlanger()
{
}

void Engine::MonoFlanger::init(const float _samplerate, const uint32_t _upsampleFactor)
{
  // outputs
  m_out_L = 0.0f;
  m_out_R = 0.0f;
  // prewarp
  m_warpConst_PI = NlToolbox::Constants::pi / _samplerate;
  m_warpConst_2PI = NlToolbox::Constants::twopi / _samplerate;
  m_freqClip_min = _samplerate / 24576.0f;
  m_freqClip_max = _samplerate / 2.125f;
  // 1p lp
  m_lp_b0 = 0.0f;
  m_lp_b1 = 0.0f;
  m_lp_a1 = 0.0f;
  m_lp_stateVar_L1 = 0.0f;
  m_lp_stateVar_L2 = 0.0f;
  m_lp_stateVar_R1 = 0.0f;
  m_lp_stateVar_R2 = 0.0f;
  // 1p hp
  float omega = NlToolbox::Math::tan(50.0f * m_warpConst_PI);
  float denom = 1.0f / (1.0f + omega);
  m_hp_a1 = (1.0f - omega) * denom;
  m_hp_b0 = denom;
  m_hp_b1 = -denom;
  m_hp_stateVar_L1 = 0.0f;
  m_hp_stateVar_L2 = 0.0f;
  m_hp_stateVar_R1 = 0.0f;
  m_hp_stateVar_R2 = 0.0f;
  // 2 Hz lp
  m_lp2hz_b0 = 2.0f * (NlToolbox::Constants::twopi / _samplerate);
  m_lp2hz_b0 = std::min(m_lp2hz_b0, 1.9f);
  m_lp2hz_stateVar_TL = 0.0f;
  m_lp2hz_stateVar_TR = 0.0f;
  m_lp2hz_stateVar_D = 0.0f;
  // 4p ap
  m_ap_b0_L = 0.0f;
  m_ap_b1_L = 0.0f;
  m_ap_b0_R = 0.0f;
  m_ap_b1_R = 0.0f;
  m_ap_StateVar_L1 = 0.0f;
  m_ap_StateVar_L2 = 0.0f;
  m_ap_StateVar_L3 = 0.0f;
  m_ap_StateVar_L4 = 0.0f;
  m_ap_StateVar_L5 = 0.0f;
  m_ap_StateVar_L6 = 0.0f;
  m_ap_StateVar_L7 = 0.0f;
  m_ap_StateVar_L8 = 0.0f;
  m_ap_StateVar_R1 = 0.0f;
  m_ap_StateVar_R2 = 0.0f;
  m_ap_StateVar_R3 = 0.0f;
  m_ap_StateVar_R4 = 0.0f;
  m_ap_StateVar_R5 = 0.0f;
  m_ap_StateVar_R6 = 0.0f;
  m_ap_StateVar_R7 = 0.0f;
  m_ap_StateVar_R8 = 0.0f;
  // delay buffer
  m_stateVar_L = 0.0f;
  m_stateVar_R = 0.0f;
  m_buffer_indx = 0;
  m_buffer_sz_m1 = FLANGER_BUFFER_SIZE * _upsampleFactor - 1;
  m_buffer_L.resize(FLANGER_BUFFER_SIZE * _upsampleFactor);
  std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.0f);
  m_buffer_R.resize(FLANGER_BUFFER_SIZE * _upsampleFactor);
  std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.0f);
}

void Engine::MonoFlanger::set_slow(MonoSignals &_signals)
{
  float omega = std::clamp(_signals.get(C15::Signals::Mono_Signals::Flanger_LPF), m_freqClip_min, m_freqClip_max);
  omega = NlToolbox::Math::tan(omega * m_warpConst_PI);
  float denom = 1.0f / (1.0f + omega);
  m_lp_a1 = (1.0f - omega) * denom;
  m_lp_b0 = m_lp_b1 = omega * denom;
}

void Engine::MonoFlanger::set_fast(MonoSignals &_signals)
{
  // ap l
  float tmpVar = _signals.get(C15::Signals::Mono_Signals::Flanger_APF_L);
  tmpVar = std::clamp(tmpVar, m_freqClip_min, m_freqClip_max);
  tmpVar *= m_warpConst_2PI;
  m_ap_b1_L = NlToolbox::Math::cos(tmpVar) * -2.0f;
  tmpVar = NlToolbox::Math::sin(tmpVar);
  m_ap_b0_L = 1.0f - tmpVar;
  tmpVar = 1.0f / (1.0f + tmpVar);
  m_ap_b1_L *= tmpVar;
  m_ap_b0_L *= tmpVar;
  // ap r
  tmpVar = _signals.get(C15::Signals::Mono_Signals::Flanger_APF_R);
  tmpVar = std::clamp(tmpVar, m_freqClip_min, m_freqClip_max);
  tmpVar *= m_warpConst_2PI;
  m_ap_b1_R = NlToolbox::Math::cos(tmpVar) * -2.0f;
  tmpVar = NlToolbox::Math::sin(tmpVar);
  m_ap_b0_R = 1.0f - tmpVar;
  tmpVar = 1.0f / (1.0f + tmpVar);
  m_ap_b1_R *= tmpVar;
  m_ap_b0_R *= tmpVar;
}

void Engine::MonoFlanger::apply(MonoSignals &_signals, const float _rawSample_L, const float _rawSample_R)
{
  float tmpVar_1, tmpVar_2;
  // 2 Hz lp tmod
  float tmod = _signals.get(C15::Signals::Mono_Signals::Flanger_Time_Mod) - m_lp2hz_stateVar_D;
  tmod = tmod * m_lp2hz_b0 + m_lp2hz_stateVar_D;
  m_lp2hz_stateVar_D = tmod;
  // left channel
  tmpVar_1 = _rawSample_L + (m_stateVar_L * _signals.get(C15::Signals::Mono_Signals::Flanger_FB_Local))
      + (m_stateVar_R * _signals.get(C15::Signals::Mono_Signals::Flanger_FB_Cross));
  // lp l
  tmpVar_2 = m_lp_b0 * tmpVar_1;
  tmpVar_2 += m_lp_b1 * m_lp_stateVar_L1;
  tmpVar_2 += m_lp_a1 * m_lp_stateVar_L2;
  m_lp_stateVar_L1 = tmpVar_1;
  m_lp_stateVar_L2 = tmpVar_2;
  m_buffer_L[m_buffer_indx] = tmpVar_2;
  // 2 Hz lp tl
  tmpVar_1 = _signals.get(C15::Signals::Mono_Signals::Flanger_Time_L) - m_lp2hz_stateVar_TL;
  tmpVar_1 = tmpVar_1 * m_lp2hz_b0 + m_lp2hz_stateVar_TL;
  m_lp2hz_stateVar_TL = tmpVar_1;
  tmpVar_1 = tmpVar_1 + tmpVar_1 * tmod * _signals.get(C15::Signals::Mono_Signals::Flanger_LFO_L);
  int32_t ind_t0 = static_cast<int32_t>(std::round(tmpVar_1 - 0.5f));
  tmpVar_1 = tmpVar_1 - static_cast<float>(ind_t0);
  int32_t ind_tm1 = std::max(ind_t0 - 1, 0);
  int32_t ind_tp1 = ind_t0 + 1;
  int32_t ind_tp2 = ind_t0 + 2;
  ind_tm1 = m_buffer_indx - ind_tm1;
  ind_t0 = m_buffer_indx - ind_t0;
  ind_tp1 = m_buffer_indx - ind_tp1;
  ind_tp2 = m_buffer_indx - ind_tp2;
  ind_tm1 &= m_buffer_sz_m1;
  ind_t0 &= m_buffer_sz_m1;
  ind_tp1 &= m_buffer_sz_m1;
  ind_tp2 &= m_buffer_sz_m1;
  tmpVar_1 = NlToolbox::Math::interpolRT(tmpVar_1, m_buffer_L[ind_tm1], m_buffer_L[ind_t0], m_buffer_L[ind_tp1],
                                         m_buffer_L[ind_tp2]);
  // ap l
  tmpVar_2 = tmpVar_1 * m_ap_b0_L;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_L1 * m_ap_b1_L;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_L2;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_L3 * m_ap_b1_L;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_L4 * m_ap_b0_L;
  m_ap_StateVar_L2 = m_ap_StateVar_L1;
  m_ap_StateVar_L1 = tmpVar_1;
  tmpVar_1 = tmpVar_2;
  m_ap_StateVar_L4 = m_ap_StateVar_L3;
  m_ap_StateVar_L3 = tmpVar_2;
  tmpVar_2 = tmpVar_2 * m_ap_b0_L;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_L5 * m_ap_b1_L;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_L6;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_L7 * m_ap_b1_L;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_L8 * m_ap_b0_L;
  m_ap_StateVar_L6 = m_ap_StateVar_L5;
  m_ap_StateVar_L5 = tmpVar_1;
  m_ap_StateVar_L8 = m_ap_StateVar_L7;
  m_ap_StateVar_L7 = tmpVar_2;
  // hp l
  m_stateVar_L = m_hp_b0 * tmpVar_2;
  m_stateVar_L += m_hp_b1 * m_hp_stateVar_L1;
  m_stateVar_L += m_hp_a1 * m_hp_stateVar_L2;
  m_hp_stateVar_L1 = tmpVar_2;
  m_hp_stateVar_L2 = m_stateVar_L;
  // out & tremolo l
  m_out_L
      = NlToolbox::Crossfades::crossFade(_rawSample_L, tmpVar_2, _signals.get(C15::Signals::Mono_Signals::Flanger_DRY),
                                         _signals.get(C15::Signals::Mono_Signals::Flanger_WET));
  m_out_L *= 1.0f
      + (_signals.get(C15::Signals::Mono_Signals::Flanger_Tremolo_L)
         * _signals.get(C15::Signals::Mono_Signals::Flanger_Tremolo));
  // right channel
  tmpVar_1 = _rawSample_R + (m_stateVar_R * _signals.get(C15::Signals::Mono_Signals::Flanger_FB_Local))
      + (m_stateVar_L * _signals.get(C15::Signals::Mono_Signals::Flanger_FB_Cross));
  // lp r
  tmpVar_2 = m_lp_b0 * tmpVar_1;
  tmpVar_2 += m_lp_b1 * m_lp_stateVar_R1;
  tmpVar_2 += m_lp_a1 * m_lp_stateVar_R2;
  m_lp_stateVar_R1 = tmpVar_1;
  m_lp_stateVar_R2 = tmpVar_2;
  m_buffer_R[m_buffer_indx] = tmpVar_2;
  // 2 Hz lp tr
  tmpVar_1 = _signals.get(C15::Signals::Mono_Signals::Flanger_Time_R) - m_lp2hz_stateVar_TR;
  tmpVar_1 = tmpVar_1 * m_lp2hz_b0 + m_lp2hz_stateVar_TR;
  m_lp2hz_stateVar_TR = tmpVar_1;
  tmpVar_1 = tmpVar_1 + tmpVar_1 * tmod * _signals.get(C15::Signals::Mono_Signals::Flanger_LFO_R);
  ind_t0 = static_cast<int32_t>(std::round(tmpVar_1 - 0.5f));
  tmpVar_1 = tmpVar_1 - static_cast<float>(ind_t0);
  ind_tm1 = std::max(ind_t0 - 1, 0);
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
  tmpVar_1 = NlToolbox::Math::interpolRT(tmpVar_1, m_buffer_R[ind_tm1], m_buffer_R[ind_t0], m_buffer_R[ind_tp1],
                                         m_buffer_R[ind_tp2]);
  // ap r
  tmpVar_2 = tmpVar_1 * m_ap_b0_R;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_R1 * m_ap_b1_R;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_R2;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_R3 * m_ap_b1_R;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_R4 * m_ap_b0_R;
  m_ap_StateVar_R2 = m_ap_StateVar_R1;
  m_ap_StateVar_R1 = tmpVar_1;
  tmpVar_1 = tmpVar_2;
  m_ap_StateVar_R4 = m_ap_StateVar_R3;
  m_ap_StateVar_R3 = tmpVar_2;
  tmpVar_2 = tmpVar_2 * m_ap_b0_R;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_R5 * m_ap_b1_R;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_R6;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_R7 * m_ap_b1_R;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_R8 * m_ap_b0_R;
  m_ap_StateVar_R6 = m_ap_StateVar_R5;
  m_ap_StateVar_R5 = tmpVar_1;
  m_ap_StateVar_R8 = m_ap_StateVar_R7;
  m_ap_StateVar_R7 = tmpVar_2;
  // hp r
  m_stateVar_R = m_hp_b0 * tmpVar_2;
  m_stateVar_R += m_hp_b1 * m_hp_stateVar_R1;
  m_stateVar_R += m_hp_a1 * m_hp_stateVar_R2;
  m_hp_stateVar_R1 = tmpVar_2;
  m_hp_stateVar_R2 = m_stateVar_R;
  // out & tremolo r
  m_out_R
      = NlToolbox::Crossfades::crossFade(_rawSample_R, tmpVar_2, _signals.get(C15::Signals::Mono_Signals::Flanger_DRY),
                                         _signals.get(C15::Signals::Mono_Signals::Flanger_WET));
  m_out_R *= 1.0f
      + (_signals.get(C15::Signals::Mono_Signals::Flanger_Tremolo_R)
         * _signals.get(C15::Signals::Mono_Signals::Flanger_Tremolo));
  // ringbuffer index update
  m_buffer_indx = (m_buffer_indx + 1) & m_buffer_sz_m1;
}

void Engine::MonoFlanger::resetDSP()
{
  // outputs
  m_out_L = 0.0f;
  m_out_R = 0.0f;
  // 1p lp
  m_lp_stateVar_L1 = 0.0f;
  m_lp_stateVar_L2 = 0.0f;
  m_lp_stateVar_R1 = 0.0f;
  m_lp_stateVar_R2 = 0.0f;
  // 1p hp
  m_hp_stateVar_L1 = 0.0f;
  m_hp_stateVar_L2 = 0.0f;
  m_hp_stateVar_R1 = 0.0f;
  m_hp_stateVar_R2 = 0.0f;
  // 2 Hz lp
  m_lp2hz_stateVar_TL = 0.0f;
  m_lp2hz_stateVar_TR = 0.0f;
  m_lp2hz_stateVar_D = 0.0f;
  // 4p ap
  m_ap_StateVar_L1 = 0.0f;
  m_ap_StateVar_L2 = 0.0f;
  m_ap_StateVar_L3 = 0.0f;
  m_ap_StateVar_L4 = 0.0f;
  m_ap_StateVar_L5 = 0.0f;
  m_ap_StateVar_L6 = 0.0f;
  m_ap_StateVar_L7 = 0.0f;
  m_ap_StateVar_L8 = 0.0f;
  m_ap_StateVar_R1 = 0.0f;
  m_ap_StateVar_R2 = 0.0f;
  m_ap_StateVar_R3 = 0.0f;
  m_ap_StateVar_R4 = 0.0f;
  m_ap_StateVar_R5 = 0.0f;
  m_ap_StateVar_R6 = 0.0f;
  m_ap_StateVar_R7 = 0.0f;
  m_ap_StateVar_R8 = 0.0f;
  // delay buffer
  m_stateVar_L = 0.0f;
  m_stateVar_R = 0.0f;
  std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.0f);
  std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.0f);
}
