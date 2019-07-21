/******************************************************************************/
/** @file           ae_echo.cpp
    @date           2018-07-26
    @version        1.0
    @author         Anton Schmied
    @brief          Echo
    @todo
*******************************************************************************/

#include "ae_echo.h"
#include "SignalStorage.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_echo::ae_echo()
{
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_echo::init(float _samplerate, uint32_t _upsampleFactor)
{
  m_out_L = 0.f;
  m_out_R = 0.f;

  m_warpConst_PI = pi / static_cast<float>(_samplerate);
  m_freqClip_min = static_cast<float>(_samplerate) / 24576.f;
  m_freqClip_max = static_cast<float>(_samplerate) / 2.125f;

  //*************************** 1 pole Highpass ****************************//
  float omega = NlToolbox::Math::tan(50.f * m_warpConst_PI);
  m_hp_a1 = (1.f - omega) / (1.f + omega);
  m_hp_b0 = 1.f / (1.f + omega);
  m_hp_b1 = (1.f / (1.f + omega)) * -1.f;

  m_hp_stateVar_L1 = 0.f;
  m_hp_stateVar_L2 = 0.f;
  m_hp_stateVar_R1 = 0.f;
  m_hp_stateVar_R2 = 0.f;

  //**************************** 1 pole Lowpass ****************************//
  m_lp_b0 = 0.f;
  m_lp_b1 = 0.f;
  m_lp_a1 = 0.f;

  m_lp_stateVar_L1 = 0.f;
  m_lp_stateVar_L2 = 0.f;
  m_lp_stateVar_R1 = 0.f;
  m_lp_stateVar_R2 = 0.f;

  //****************************** 2Hz Lowpass *****************************//
  m_lp2hz_b0 = 2.f * (twopi / static_cast<float>(_samplerate));
  m_lp2hz_b0 = std::min(m_lp2hz_b0, 1.9f);

  m_lp2hz_stateVar_L = 0.f;
  m_lp2hz_stateVar_R = 0.f;

  //***************************** Delay Buffer *****************************//
  m_stateVar_L = 0.f;
  m_stateVar_R = 0.f;

  m_buffer_indx = 0;
  m_buffer_sz_m1 = ECHO_BUFFER_SIZE * _upsampleFactor - 1;
  m_buffer_L.resize(ECHO_BUFFER_SIZE * _upsampleFactor);
  std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.f);
  m_buffer_R.resize(ECHO_BUFFER_SIZE * _upsampleFactor);
  std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.f);
}

/******************************************************************************/
/** @brief
*******************************************************************************/
void ae_echo::set(SignalStorage &signals)
{
  float omega = std::clamp(signals.get<Signals::DLY_LPF>(), m_freqClip_min, m_freqClip_max);
  omega = NlToolbox::Math::tan(omega * m_warpConst_PI);

  m_lp_a1 = (1.f - omega) / (1.f + omega);
  m_lp_b0 = omega / (1.f + omega);
  m_lp_b1 = omega / (1.f + omega);
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_echo::apply(float _rawSample_L, float _rawSample_R, SignalStorage &signals)
{
  float tmpVar;

  //***************************** Left Channel *****************************//
  tmpVar = (_rawSample_L * signals.get<Signals::DLY_SND>()) + (m_stateVar_L * signals.get<Signals::DLY_FB_LOC>())
      + (m_stateVar_R * signals.get<Signals::DLY_FB_CR>());

  m_buffer_L[m_buffer_indx] = tmpVar;

  tmpVar = signals.get<Signals::DLY_TL>() - m_lp2hz_stateVar_L;  // 2Hz LP
  tmpVar = tmpVar * m_lp2hz_b0 + m_lp2hz_stateVar_L;

  m_lp2hz_stateVar_L = tmpVar + DNC_const;

  int32_t ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
  tmpVar = tmpVar - static_cast<float>(ind_t0);

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

  tmpVar = NlToolbox::Math::interpolRT(tmpVar, m_buffer_L[ind_tm1], m_buffer_L[ind_t0], m_buffer_L[ind_tp1],
                                       m_buffer_L[ind_tp2]);

  m_out_L = m_lp_b0 * tmpVar;  // LP
  m_out_L += m_lp_b1 * m_lp_stateVar_L1;
  m_out_L += m_lp_a1 * m_lp_stateVar_L2;

  m_lp_stateVar_L1 = tmpVar + DNC_const;
  m_lp_stateVar_L2 = m_out_L + DNC_const;

  m_stateVar_L = m_hp_b0 * m_out_L;  // HP
  m_stateVar_L += m_hp_b1 * m_hp_stateVar_L1;
  m_stateVar_L += m_hp_a1 * m_hp_stateVar_L2;

  m_hp_stateVar_L1 = m_out_L + DNC_const;
  m_hp_stateVar_L2 = m_stateVar_L + DNC_const;

  m_stateVar_L += DNC_const;

  m_out_L = NlToolbox::Crossfades::crossFade(_rawSample_L, m_out_L, signals.get<Signals::DLY_DRY>(),
                                             signals.get<Signals::DLY_WET>());

  //**************************** Right Channel *****************************//
  tmpVar = (_rawSample_R * signals.get<Signals::DLY_SND>()) + (m_stateVar_R * signals.get<Signals::DLY_FB_LOC>())
      + (m_stateVar_L * signals.get<Signals::DLY_FB_CR>());

  m_buffer_R[m_buffer_indx] = tmpVar;

  tmpVar = signals.get<Signals::DLY_TR>() - m_lp2hz_stateVar_R;  // 2Hz LP
  tmpVar = tmpVar * m_lp2hz_b0 + m_lp2hz_stateVar_R;

  m_lp2hz_stateVar_R = tmpVar + DNC_const;

  ind_t0 = static_cast<int32_t>(std::round(tmpVar - 0.5f));
  tmpVar = tmpVar - static_cast<float>(ind_t0);

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

  tmpVar = NlToolbox::Math::interpolRT(tmpVar, m_buffer_R[ind_tm1], m_buffer_R[ind_t0], m_buffer_R[ind_tp1],
                                       m_buffer_R[ind_tp2]);

  m_out_R = m_lp_b0 * tmpVar;  // LP
  m_out_R += m_lp_b1 * m_lp_stateVar_R1;
  m_out_R += m_lp_a1 * m_lp_stateVar_R2;

  m_lp_stateVar_R1 = tmpVar + DNC_const;
  m_lp_stateVar_R2 = m_out_R + DNC_const;

  m_stateVar_R = m_hp_b0 * m_out_R;  // HP
  m_stateVar_R += m_hp_b1 * m_hp_stateVar_R1;
  m_stateVar_R += m_hp_a1 * m_hp_stateVar_R2;

  m_hp_stateVar_R1 = m_out_R + DNC_const;
  m_hp_stateVar_R2 = m_stateVar_R + DNC_const;

  m_stateVar_R += DNC_const;  /// Brauchen wir das wirklich?

  m_out_R = NlToolbox::Crossfades::crossFade(_rawSample_R, m_out_R, signals.get<Signals::DLY_DRY>(),
                                             signals.get<Signals::DLY_WET>());

  m_buffer_indx = (m_buffer_indx + 1) & m_buffer_sz_m1;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_echo::resetDSP()
{
  m_out_L = 0.f;
  m_out_R = 0.f;

  //*************************** 1 pole Highpass ****************************//

  m_hp_stateVar_L1 = 0.f;
  m_hp_stateVar_L2 = 0.f;
  m_hp_stateVar_R1 = 0.f;
  m_hp_stateVar_R2 = 0.f;

  //**************************** 1 pole Lowpass ****************************//

  m_lp_stateVar_L1 = 0.f;
  m_lp_stateVar_L2 = 0.f;
  m_lp_stateVar_R1 = 0.f;
  m_lp_stateVar_R2 = 0.f;

  //****************************** 2Hz Lowpass *****************************//

  m_lp2hz_stateVar_L = 0.f;
  m_lp2hz_stateVar_R = 0.f;

  //***************************** Delay Buffer *****************************//
  m_stateVar_L = 0.f;
  m_stateVar_R = 0.f;

  std::fill(m_buffer_L.begin(), m_buffer_L.end(), 0.f);
  std::fill(m_buffer_R.begin(), m_buffer_R.end(), 0.f);
}
