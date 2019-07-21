/******************************************************************************/
/** @file           ae_flanger.cpp
    @date           2018-07-26
    @version        1.0
    @author         Anton Schmied
    @brief          Flanger
    @todo
*******************************************************************************/

#include "ae_flanger.h"
#include "SignalStorage.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_flanger::ae_flanger()
{
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_flanger::init(float _samplerate, uint32_t _upsampleFactor)
{
  m_out_L = 0.f;
  m_out_R = 0.f;

  m_warpConst_PI = pi / static_cast<float>(_samplerate);
  m_warpConst_2PI = twopi / static_cast<float>(_samplerate);
  m_freqClip_min = static_cast<float>(_samplerate) / 24576.f;
  m_freqClip_max = static_cast<float>(_samplerate) / 2.125f;

  //**************************** 1 pole Lowpass ****************************//
  m_lp_b0 = 0.f;
  m_lp_b1 = 0.f;
  m_lp_a1 = 0.f;

  m_lp_stateVar_L1 = 0.f;
  m_lp_stateVar_L2 = 0.f;
  m_lp_stateVar_R1 = 0.f;
  m_lp_stateVar_R2 = 0.f;

  //*************************** 1 pole Highpass ****************************//
  float omega = NlToolbox::Math::tan(50.f * m_warpConst_PI);
  m_hp_a1 = (1.f - omega) / (1.f + omega);
  m_hp_b0 = 1.f / (1.f + omega);
  m_hp_b1 = (1.f / (1.f + omega)) * -1.f;

  m_hp_stateVar_L1 = 0.f;
  m_hp_stateVar_L2 = 0.f;
  m_hp_stateVar_R1 = 0.f;
  m_hp_stateVar_R2 = 0.f;

  //****************************** 2Hz Lowpass *****************************//
  m_lp2hz_b0 = 2.f * (twopi / static_cast<float>(_samplerate));
  m_lp2hz_b0 = std::min(m_lp2hz_b0, 1.9f);

  m_lp2hz_stateVar_TL = 0.f;
  m_lp2hz_stateVar_TR = 0.f;
  m_lp2hz_stateVar_D = 0.f;

  //**************************** 4 Pole Allpass ****************************//
  m_ap_b0_L = 0.f;
  m_ap_b1_L = 0.f;

  m_ap_b0_R = 0.f;
  m_ap_b1_R = 0.f;

  m_ap_StateVar_L1 = 0.f;
  m_ap_StateVar_L2 = 0.f;
  m_ap_StateVar_L3 = 0.f;
  m_ap_StateVar_L4 = 0.f;
  m_ap_StateVar_L5 = 0.f;
  m_ap_StateVar_L6 = 0.f;
  m_ap_StateVar_L7 = 0.f;
  m_ap_StateVar_L8 = 0.f;

  m_ap_StateVar_R1 = 0.f;
  m_ap_StateVar_R2 = 0.f;
  m_ap_StateVar_R3 = 0.f;
  m_ap_StateVar_R4 = 0.f;
  m_ap_StateVar_R5 = 0.f;
  m_ap_StateVar_R6 = 0.f;
  m_ap_StateVar_R7 = 0.f;
  m_ap_StateVar_R8 = 0.f;

  //***************************** Delay Buffer *****************************//
  m_stateVar_L = 0.f;
  m_stateVar_R = 0.f;

  m_buffer_indx = 0;
  m_buffer_sz_m1 = FLANGER_BUFFER_SIZE * _upsampleFactor - 1;

  m_buffer_L.resize(FLANGER_BUFFER_SIZE * _upsampleFactor);
  std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.f);
  m_buffer_R.resize(FLANGER_BUFFER_SIZE * _upsampleFactor);
  std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.f);
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_flanger::set_slow(SignalStorage &signals)
{
  float omega = std::clamp(signals.get<Signals::FLA_LPF>(), m_freqClip_min, m_freqClip_max);
  omega = NlToolbox::Math::tan(omega * m_warpConst_PI);

  m_lp_a1 = (1.f - omega) / (1.f + omega);
  m_lp_b0 = omega / (1.f + omega);
  m_lp_b1 = omega / (1.f + omega);
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_flanger::set_fast(SignalStorage &signals)
{
  float tmpVar = signals.get<Signals::FLA_APF_L>();  // AP L
  tmpVar = std::clamp(tmpVar, m_freqClip_min, m_freqClip_max);

  tmpVar *= m_warpConst_2PI;

  m_ap_b1_L = NlToolbox::Math::cos(tmpVar) * -2.f;

  tmpVar = NlToolbox::Math::sin(tmpVar);
  m_ap_b0_L = 1.f - tmpVar;

  tmpVar = 1.f / (1.f + tmpVar);

  m_ap_b1_L *= tmpVar;
  m_ap_b0_L *= tmpVar;

  tmpVar = signals.get<Signals::FLA_APF_R>();  // AP R
  tmpVar = std::clamp(tmpVar, m_freqClip_min, m_freqClip_max);

  tmpVar *= m_warpConst_2PI;

  m_ap_b1_R = NlToolbox::Math::cos(tmpVar) * -2.f;

  tmpVar = NlToolbox::Math::sin(tmpVar);
  m_ap_b0_R = 1.f - tmpVar;

  tmpVar = 1.f / (1.f + tmpVar);

  m_ap_b1_R *= tmpVar;
  m_ap_b0_R *= tmpVar;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_flanger::apply(float _rawSample_L, float _rawSample_R, SignalStorage &signals)
{
  float tmpVar_1, tmpVar_2;

  float tmod = signals.get<Signals::FLA_TMOD>() - m_lp2hz_stateVar_D;  // 2Hz LP TMOD
  tmod = tmod * m_lp2hz_b0 + m_lp2hz_stateVar_D;

  m_lp2hz_stateVar_D = tmod + DNC_const;

  //**************************** Left Channel *****************************//
  tmpVar_1 = _rawSample_L + (m_stateVar_L * signals.get<Signals::FLA_FB_LOC>())
      + (m_stateVar_R * signals.get<Signals::FLA_FB_CR>());

  tmpVar_2 = m_lp_b0 * tmpVar_1;  // LP L
  tmpVar_2 += m_lp_b1 * m_lp_stateVar_L1;
  tmpVar_2 += m_lp_a1 * m_lp_stateVar_L2;

  m_lp_stateVar_L1 = tmpVar_1 + DNC_const;
  m_lp_stateVar_L2 = tmpVar_2 + DNC_const;

  m_buffer_L[m_buffer_indx] = tmpVar_2;

  tmpVar_1 = signals.get<Signals::FLA_TL>() - m_lp2hz_stateVar_TL;  // 2Hz LP TL
  tmpVar_1 = tmpVar_1 * m_lp2hz_b0 + m_lp2hz_stateVar_TL;

  m_lp2hz_stateVar_TL = tmpVar_1 + DNC_const;

  tmpVar_1 = tmpVar_1 + tmpVar_1 * tmod * signals.get<Signals::FLA_LFO_L>();

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

  tmpVar_2 = tmpVar_1 * m_ap_b0_L;  // AP L
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_L1 * m_ap_b1_L;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_L2;

  tmpVar_2 = tmpVar_2 - m_ap_StateVar_L3 * m_ap_b1_L;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_L4 * m_ap_b0_L;

  m_ap_StateVar_L2 = m_ap_StateVar_L1;
  m_ap_StateVar_L1 = tmpVar_1;
  tmpVar_1 = tmpVar_2;
  m_ap_StateVar_L4 = m_ap_StateVar_L3;
  m_ap_StateVar_L3 = tmpVar_2 + DNC_const;

  tmpVar_2 = tmpVar_2 * m_ap_b0_L;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_L5 * m_ap_b1_L;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_L6;

  tmpVar_2 = tmpVar_2 - m_ap_StateVar_L7 * m_ap_b1_L;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_L8 * m_ap_b0_L;

  m_ap_StateVar_L6 = m_ap_StateVar_L5;
  m_ap_StateVar_L5 = tmpVar_1;
  m_ap_StateVar_L8 = m_ap_StateVar_L7;
  m_ap_StateVar_L7 = tmpVar_2 + DNC_const;

  m_stateVar_L = m_hp_b0 * tmpVar_2;  // HP L
  m_stateVar_L += m_hp_b1 * m_hp_stateVar_L1;
  m_stateVar_L += m_hp_a1 * m_hp_stateVar_L2;

  m_hp_stateVar_L1 = tmpVar_2 + DNC_const;
  m_hp_stateVar_L2 = m_stateVar_L + DNC_const;

  m_stateVar_L += DNC_const;

  m_out_L = NlToolbox::Crossfades::crossFade(_rawSample_L, tmpVar_2, signals.get<Signals::FLA_DRY>(),
                                             signals.get<Signals::FLA_WET>());

  //*************************** Right Channel *****************************//
  tmpVar_1 = _rawSample_R + (m_stateVar_R * signals.get<Signals::FLA_FB_LOC>())
      + (m_stateVar_L * signals.get<Signals::FLA_FB_CR>());

  tmpVar_2 = m_lp_b0 * tmpVar_1;  // LP L
  tmpVar_2 += m_lp_b1 * m_lp_stateVar_R1;
  tmpVar_2 += m_lp_a1 * m_lp_stateVar_R2;

  m_lp_stateVar_R1 = tmpVar_1 + DNC_const;
  m_lp_stateVar_R2 = tmpVar_2 + DNC_const;

  m_buffer_R[m_buffer_indx] = tmpVar_2;

  tmpVar_1 = signals.get<Signals::FLA_TR>() - m_lp2hz_stateVar_TR;  // 2Hz LP TR
  tmpVar_1 = tmpVar_1 * m_lp2hz_b0 + m_lp2hz_stateVar_TR;

  m_lp2hz_stateVar_TR = tmpVar_1 + DNC_const;

  tmpVar_1 = tmpVar_1 + tmpVar_1 * tmod * signals.get<Signals::FLA_LFO_R>();

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

  tmpVar_2 = tmpVar_1 * m_ap_b0_R;  // AP R
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_R1 * m_ap_b1_R;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_R2;

  tmpVar_2 = tmpVar_2 - m_ap_StateVar_R3 * m_ap_b1_R;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_R4 * m_ap_b0_R;

  m_ap_StateVar_R2 = m_ap_StateVar_R1;
  m_ap_StateVar_R1 = tmpVar_1;
  tmpVar_1 = tmpVar_2;
  m_ap_StateVar_R4 = m_ap_StateVar_R3;
  m_ap_StateVar_R3 = tmpVar_2 + DNC_const;

  tmpVar_2 = tmpVar_2 * m_ap_b0_R;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_R5 * m_ap_b1_R;
  tmpVar_2 = tmpVar_2 + m_ap_StateVar_R6;

  tmpVar_2 = tmpVar_2 - m_ap_StateVar_R7 * m_ap_b1_R;
  tmpVar_2 = tmpVar_2 - m_ap_StateVar_R8 * m_ap_b0_R;

  m_ap_StateVar_R6 = m_ap_StateVar_R5;
  m_ap_StateVar_R5 = tmpVar_1;
  m_ap_StateVar_R8 = m_ap_StateVar_R7;
  m_ap_StateVar_R7 = tmpVar_2 + DNC_const;

  m_stateVar_R = m_hp_b0 * tmpVar_2;  // HP R
  m_stateVar_R += m_hp_b1 * m_hp_stateVar_R1;
  m_stateVar_R += m_hp_a1 * m_hp_stateVar_R2;

  m_hp_stateVar_R1 = tmpVar_2 + DNC_const;
  m_hp_stateVar_R2 = m_stateVar_R + DNC_const;

  m_stateVar_R += DNC_const;

  m_out_R = NlToolbox::Crossfades::crossFade(_rawSample_R, tmpVar_2, signals.get<Signals::FLA_DRY>(),
                                             signals.get<Signals::FLA_WET>());

  m_buffer_indx = (m_buffer_indx + 1) & m_buffer_sz_m1;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_flanger::resetDSP()
{
  m_out_L = 0.f;
  m_out_R = 0.f;

  //**************************** 1 pole Lowpass ****************************//

  m_lp_stateVar_L1 = 0.f;
  m_lp_stateVar_L2 = 0.f;
  m_lp_stateVar_R1 = 0.f;
  m_lp_stateVar_R2 = 0.f;

  //*************************** 1 pole Highpass ****************************//

  m_hp_stateVar_L1 = 0.f;
  m_hp_stateVar_L2 = 0.f;
  m_hp_stateVar_R1 = 0.f;
  m_hp_stateVar_R2 = 0.f;

  //****************************** 2Hz Lowpass *****************************//

  m_lp2hz_stateVar_TL = 0.f;
  m_lp2hz_stateVar_TR = 0.f;
  m_lp2hz_stateVar_D = 0.f;

  //**************************** 4 Pole Allpass ****************************//

  m_ap_StateVar_L1 = 0.f;
  m_ap_StateVar_L2 = 0.f;
  m_ap_StateVar_L3 = 0.f;
  m_ap_StateVar_L4 = 0.f;
  m_ap_StateVar_L5 = 0.f;
  m_ap_StateVar_L6 = 0.f;
  m_ap_StateVar_L7 = 0.f;
  m_ap_StateVar_L8 = 0.f;

  m_ap_StateVar_R1 = 0.f;
  m_ap_StateVar_R2 = 0.f;
  m_ap_StateVar_R3 = 0.f;
  m_ap_StateVar_R4 = 0.f;
  m_ap_StateVar_R5 = 0.f;
  m_ap_StateVar_R6 = 0.f;
  m_ap_StateVar_R7 = 0.f;
  m_ap_StateVar_R8 = 0.f;

  //***************************** Delay Buffer *****************************//
  m_stateVar_L = 0.f;
  m_stateVar_R = 0.f;

  std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.f);
  std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.f);
}
