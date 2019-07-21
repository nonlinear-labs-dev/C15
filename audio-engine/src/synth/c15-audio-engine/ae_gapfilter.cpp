/******************************************************************************/
/** @file           ae_gapfilter.cpp
    @date           2018-07-25
    @version        1.0
    @author         Anton Schmied
    @brief          Gapfilter
    @todo
*******************************************************************************/

#include "ae_gapfilter.h"
#include "SignalStorage.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_gapfilter::ae_gapfilter()
{
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_gapfilter::init(float _samplerate)
{
  m_out_L = 0.f;
  m_out_R = 0.f;

  m_warpConst_2PI = 6.28319f / _samplerate;
  m_freqClip_max = _samplerate / 2.125f;
  m_freqClip_min = _samplerate / 24576.f;
  m_freqScale_max = _samplerate / 3.f;
  m_freqScale_min = _samplerate / 2.f;

  //************************** Biquad Highpass L/R **************************//
  m_hp_l1_b0 = 0.f;
  m_hp_l1_b1 = 0.f;
  m_hp_l1_a1 = 0.f;
  m_hp_l1_a2 = 0.f;

  m_hp_l2_b0 = 0.f;
  m_hp_l2_b1 = 0.f;
  m_hp_l2_a1 = 0.f;
  m_hp_l2_a2 = 0.f;

  m_hp_l1_stateVar_1 = 0.f;
  m_hp_l1_stateVar_2 = 0.f;
  m_hp_l1_stateVar_3 = 0.f;
  m_hp_l1_stateVar_4 = 0.f;

  m_hp_l2_stateVar_1 = 0.f;
  m_hp_l2_stateVar_2 = 0.f;
  m_hp_l2_stateVar_3 = 0.f;
  m_hp_l2_stateVar_4 = 0.f;

  m_hp_r1_b0 = 0.f;
  m_hp_r1_b1 = 0.f;
  m_hp_r1_a1 = 0.f;
  m_hp_r1_a2 = 0.f;

  m_hp_r2_b0 = 0.f;
  m_hp_r2_b1 = 0.f;
  m_hp_r2_a1 = 0.f;
  m_hp_r2_a2 = 0.f;

  m_hp_r1_stateVar_1 = 0.f;
  m_hp_r1_stateVar_2 = 0.f;
  m_hp_r1_stateVar_3 = 0.f;
  m_hp_r1_stateVar_4 = 0.f;

  m_hp_r2_stateVar_1 = 0.f;
  m_hp_r2_stateVar_2 = 0.f;
  m_hp_r2_stateVar_3 = 0.f;
  m_hp_r2_stateVar_4 = 0.f;

  //*************************** Biquad Lowpass L/R **************************//
  m_lp_l1_b0 = 0.f;
  m_lp_l1_b1 = 0.f;
  m_lp_l1_a1 = 0.f;
  m_lp_l1_a2 = 0.f;

  m_lp_l2_b0 = 0.f;
  m_lp_l2_b1 = 0.f;
  m_lp_l2_a1 = 0.f;
  m_lp_l2_a2 = 0.f;

  m_lp_l1_stateVar_1 = 0.f;
  m_lp_l1_stateVar_2 = 0.f;
  m_lp_l1_stateVar_3 = 0.f;
  m_lp_l1_stateVar_4 = 0.f;

  m_lp_l2_stateVar_1 = 0.f;
  m_lp_l2_stateVar_2 = 0.f;
  m_lp_l2_stateVar_3 = 0.f;
  m_lp_l2_stateVar_4 = 0.f;

  m_lp_r1_b0 = 0.f;
  m_lp_r1_b1 = 0.f;
  m_lp_r1_a1 = 0.f;
  m_lp_r1_a2 = 0.f;

  m_lp_r2_b0 = 0.f;
  m_lp_r2_b1 = 0.f;
  m_lp_r2_a1 = 0.f;
  m_lp_r2_a2 = 0.f;

  m_lp_r1_stateVar_1 = 0.f;
  m_lp_r1_stateVar_2 = 0.f;
  m_lp_r1_stateVar_3 = 0.f;
  m_lp_r1_stateVar_4 = 0.f;

  m_lp_r2_stateVar_1 = 0.f;
  m_lp_r2_stateVar_2 = 0.f;
  m_lp_r2_stateVar_3 = 0.f;
  m_lp_r2_stateVar_4 = 0.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_gapfilter::set(SignalStorage &signals)
{
  float tmpVar_1, tmpVar_2;

  //************************** Biquad Highpass L ***************************//
  float frequency = signals.get<Signals::GAP_HFL>();
  tmpVar_1 = (frequency - m_freqScale_min) * (1.f / (m_freqScale_max - m_freqScale_min));
  tmpVar_1 = std::clamp(frequency, -1.f, 1.f);

  float resonance = signals.get<Signals::GAP_RES>() * tmpVar_1;

  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // L1
  tmpVar_1 *= m_warpConst_2PI;

  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_hp_l1_a1 = tmpVar_2 * -2.f;
  m_hp_l1_b0 = (1.f + tmpVar_2) / 2.f;
  m_hp_l1_b1 = (1.f + tmpVar_2) * -1.f;

  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.f - resonance);
  m_hp_l1_a2 = 1.f - tmpVar_2;
  tmpVar_2 = 1.f + tmpVar_2;

  m_hp_l1_a1 = m_hp_l1_a1 / tmpVar_2 * -1.f;
  m_hp_l1_a2 = m_hp_l1_a2 / tmpVar_2 * -1.f;
  m_hp_l1_b0 = m_hp_l1_b0 / tmpVar_2;
  m_hp_l1_b1 = m_hp_l1_b1 / tmpVar_2;

  frequency *= 0.75f;  // L2
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  tmpVar_1 *= m_warpConst_2PI;

  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_hp_l2_a1 = tmpVar_2 * -2.f;
  m_hp_l2_b0 = (1.f + tmpVar_2) / 2.f;
  m_hp_l2_b1 = (1.f + tmpVar_2) * -1.f;

  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.f - resonance);
  m_hp_l2_a2 = 1.f - tmpVar_2;
  tmpVar_2 = 1.f + tmpVar_2;

  m_hp_l2_a1 = m_hp_l2_a1 / tmpVar_2 * -1.f;
  m_hp_l2_a2 = m_hp_l2_a2 / tmpVar_2 * -1.f;
  m_hp_l2_b0 = m_hp_l2_b0 / tmpVar_2;
  m_hp_l2_b1 = m_hp_l2_b1 / tmpVar_2;

  //************************** Biquad Highpass R ***************************//
  frequency = signals.get<Signals::GAP_HFR>();
  tmpVar_1 = (frequency - m_freqScale_min) * (1.f / (m_freqScale_max - m_freqScale_min));
  tmpVar_1 = std::clamp(tmpVar_1, -1.f, 1.f);

  resonance = signals.get<Signals::GAP_RES>() * tmpVar_1;

  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // R1
  tmpVar_1 *= m_warpConst_2PI;

  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_hp_r1_a1 = tmpVar_2 * -2.f;
  m_hp_r1_b0 = (1.f + tmpVar_2) / 2.f;
  m_hp_r1_b1 = (1.f + tmpVar_2) * -1.f;

  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.f - resonance);
  m_hp_r1_a2 = 1.f - tmpVar_2;
  tmpVar_2 = 1.f + tmpVar_2;

  m_hp_r1_a1 = m_hp_r1_a1 / tmpVar_2 * -1.f;
  m_hp_r1_a2 = m_hp_r1_a2 / tmpVar_2 * -1.f;
  m_hp_r1_b0 = m_hp_r1_b0 / tmpVar_2;
  m_hp_r1_b1 = m_hp_r1_b1 / tmpVar_2;

  frequency *= 0.75f;  // R2
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  tmpVar_1 *= m_warpConst_2PI;

  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_hp_r2_a1 = tmpVar_2 * -2.f;
  m_hp_r2_b0 = (1.f + tmpVar_2) / 2.f;
  m_hp_r2_b1 = (1.f + tmpVar_2) * -1.f;

  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.f - resonance);
  m_hp_r2_a2 = 1.f - tmpVar_2;
  tmpVar_2 = 1.f + tmpVar_2;

  m_hp_r2_a1 = m_hp_r2_a1 / tmpVar_2 * -1.f;
  m_hp_r2_a2 = m_hp_r2_a2 / tmpVar_2 * -1.f;
  m_hp_r2_b0 = m_hp_r2_b0 / tmpVar_2;
  m_hp_r2_b1 = m_hp_r2_b1 / tmpVar_2;

  //*************************** Biquad Lowpass L ***************************//
  frequency = signals.get<Signals::GAP_LFL>();
  tmpVar_1 = (frequency - m_freqScale_min) * (1.f / (m_freqScale_max - m_freqScale_min));
  tmpVar_1 = std::clamp(tmpVar_1, -1.f, 1.f);

  resonance = signals.get<Signals::GAP_RES>() * tmpVar_1;

  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // L1
  tmpVar_1 *= m_warpConst_2PI;

  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);

  m_lp_l1_a1 = tmpVar_2 * -2.f;
  m_lp_l1_b0 = (1.f - tmpVar_2) / 2.f;
  m_lp_l1_b1 = 1.f - tmpVar_2;

  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.f - resonance);

  m_lp_l1_a2 = 1.f - tmpVar_2;
  tmpVar_2 = 1.f + tmpVar_2;

  m_lp_l1_a1 = m_lp_l1_a1 / tmpVar_2 * -1.f;
  m_lp_l1_a2 = m_lp_l1_a2 / tmpVar_2 * -1.f;
  m_lp_l1_b0 = m_lp_l1_b0 / tmpVar_2;
  m_lp_l1_b1 = m_lp_l1_b1 / tmpVar_2;

  frequency *= 1.33f;
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // L2
  tmpVar_1 *= m_warpConst_2PI;

  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);

  m_lp_l2_a1 = tmpVar_2 * -2.f;
  m_lp_l2_b0 = (1.f - tmpVar_2) / 2.f;
  m_lp_l2_b1 = 1.f - tmpVar_2;

  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.f - resonance);

  m_lp_l2_a2 = 1.f - tmpVar_2;
  tmpVar_2 = 1.f + tmpVar_2;

  m_lp_l2_a1 = m_lp_l2_a1 / tmpVar_2 * -1.f;
  m_lp_l2_a2 = m_lp_l2_a2 / tmpVar_2 * -1.f;
  m_lp_l2_b0 = m_lp_l2_b0 / tmpVar_2;
  m_lp_l2_b1 = m_lp_l2_b1 / tmpVar_2;

  //*************************** Biquad Lowpass R ***************************//
  frequency = signals.get<Signals::GAP_LFR>();
  tmpVar_1 = (frequency - m_freqScale_min) * (1.f / (m_freqScale_max - m_freqScale_min));
  tmpVar_1 = std::clamp(tmpVar_1, -1.f, 1.f);

  resonance = signals.get<Signals::GAP_RES>() * tmpVar_1;

  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // R1
  tmpVar_1 *= m_warpConst_2PI;

  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);

  m_lp_r1_a1 = tmpVar_2 * -2.f;
  m_lp_r1_b0 = (1.f - tmpVar_2) / 2.f;
  m_lp_r1_b1 = 1.f - tmpVar_2;

  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.f - resonance);

  m_lp_r1_a2 = 1.f - tmpVar_2;
  tmpVar_2 = 1.f + tmpVar_2;

  m_lp_r1_a1 = m_lp_r1_a1 / tmpVar_2 * -1.f;
  m_lp_r1_a2 = m_lp_r1_a2 / tmpVar_2 * -1.f;
  m_lp_r1_b0 = m_lp_r1_b0 / tmpVar_2;
  m_lp_r1_b1 = m_lp_r1_b1 / tmpVar_2;

  frequency *= 1.33f;
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // R2
  tmpVar_1 *= m_warpConst_2PI;

  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);

  m_lp_r2_a1 = tmpVar_2 * -2.f;
  m_lp_r2_b0 = (1.f - tmpVar_2) / 2.f;
  m_lp_r2_b1 = 1.f - tmpVar_2;

  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.f - resonance);

  m_lp_r2_a2 = 1.f - tmpVar_2;
  tmpVar_2 = 1.f + tmpVar_2;

  m_lp_r2_a1 = m_lp_r2_a1 / tmpVar_2 * -1.f;
  m_lp_r2_a2 = m_lp_r2_a2 / tmpVar_2 * -1.f;
  m_lp_r2_b0 = m_lp_r2_b0 / tmpVar_2;
  m_lp_r2_b1 = m_lp_r2_b1 / tmpVar_2;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_gapfilter::apply(float _rawSample_L, float _rawSample_R, SignalStorage &signals)
{
  float tmpVar;

  //***************************** Highpass L ******************************//
  float hp_sample = m_hp_l1_b0 * _rawSample_L;  // L1
  hp_sample += m_hp_l1_b1 * m_hp_l1_stateVar_1;
  hp_sample += m_hp_l1_b0 * m_hp_l1_stateVar_2;
  hp_sample += m_hp_l1_a1 * m_hp_l1_stateVar_3;
  hp_sample += m_hp_l1_a2 * m_hp_l1_stateVar_4;

  m_hp_l1_stateVar_2 = m_hp_l1_stateVar_1 + NlToolbox::Constants::DNC_const;
  m_hp_l1_stateVar_1 = _rawSample_L + NlToolbox::Constants::DNC_const;
  m_hp_l1_stateVar_4 = m_hp_l1_stateVar_3 + NlToolbox::Constants::DNC_const;
  m_hp_l1_stateVar_3 = hp_sample + NlToolbox::Constants::DNC_const;

  tmpVar = m_hp_l2_b0 * hp_sample;  // L2
  tmpVar += m_hp_l2_b1 * m_hp_l2_stateVar_1;
  tmpVar += m_hp_l2_b0 * m_hp_l2_stateVar_2;
  tmpVar += m_hp_l2_a1 * m_hp_l2_stateVar_3;
  tmpVar += m_hp_l2_a2 * m_hp_l2_stateVar_4;

  m_hp_l2_stateVar_2 = m_hp_l2_stateVar_1 + NlToolbox::Constants::DNC_const;
  m_hp_l2_stateVar_1 = hp_sample + NlToolbox::Constants::DNC_const;
  m_hp_l2_stateVar_4 = m_hp_l2_stateVar_3 + NlToolbox::Constants::DNC_const;
  m_hp_l2_stateVar_3 = tmpVar + NlToolbox::Constants::DNC_const;

  hp_sample = tmpVar;

  //****************************** Lowpass L ******************************//
  tmpVar = hp_sample * signals.get<Signals::GAP_HPLP>() + _rawSample_L * signals.get<Signals::GAP_INLP>();

  float lp_sample = m_lp_l1_b0 * tmpVar;  // L1
  lp_sample += m_lp_l1_b1 * m_lp_l1_stateVar_1;
  lp_sample += m_lp_l1_b0 * m_lp_l1_stateVar_2;
  lp_sample += m_lp_l1_a1 * m_lp_l1_stateVar_3;
  lp_sample += m_lp_l1_a2 * m_lp_l1_stateVar_4;

  m_lp_l1_stateVar_2 = m_lp_l1_stateVar_1 + NlToolbox::Constants::DNC_const;
  m_lp_l1_stateVar_1 = tmpVar + NlToolbox::Constants::DNC_const;
  m_lp_l1_stateVar_4 = m_lp_l1_stateVar_3 + NlToolbox::Constants::DNC_const;
  m_lp_l1_stateVar_3 = lp_sample + NlToolbox::Constants::DNC_const;

  tmpVar = m_lp_l2_b0 * lp_sample;  // L2
  tmpVar += m_lp_l2_b1 * m_lp_l2_stateVar_1;
  tmpVar += m_lp_l2_b0 * m_lp_l2_stateVar_2;
  tmpVar += m_lp_l2_a1 * m_lp_l2_stateVar_3;
  tmpVar += m_lp_l2_a2 * m_lp_l2_stateVar_4;

  m_lp_l2_stateVar_2 = m_lp_l2_stateVar_1 + NlToolbox::Constants::DNC_const;
  m_lp_l2_stateVar_1 = lp_sample + NlToolbox::Constants::DNC_const;
  m_lp_l2_stateVar_4 = m_lp_l2_stateVar_3 + NlToolbox::Constants::DNC_const;
  m_lp_l2_stateVar_3 = tmpVar + NlToolbox::Constants::DNC_const;

  lp_sample = tmpVar * signals.get<Signals::GAP_LPOUT>();
  hp_sample *= signals.get<Signals::GAP_HPOUT>();
  m_out_L = hp_sample + lp_sample + (_rawSample_L * signals.get<Signals::GAP_INOUT>());

  //***************************** Highpass R ******************************//
  hp_sample = m_hp_r1_b0 * _rawSample_R;  // R1
  hp_sample += m_hp_r1_b1 * m_hp_r1_stateVar_1;
  hp_sample += m_hp_r1_b0 * m_hp_r1_stateVar_2;
  hp_sample += m_hp_r1_a1 * m_hp_r1_stateVar_3;
  hp_sample += m_hp_r1_a2 * m_hp_r1_stateVar_4;

  m_hp_r1_stateVar_2 = m_hp_r1_stateVar_1 + NlToolbox::Constants::DNC_const;
  m_hp_r1_stateVar_1 = _rawSample_R + NlToolbox::Constants::DNC_const;
  m_hp_r1_stateVar_4 = m_hp_r1_stateVar_3 + NlToolbox::Constants::DNC_const;
  m_hp_r1_stateVar_3 = hp_sample + NlToolbox::Constants::DNC_const;

  tmpVar = m_hp_r2_b0 * hp_sample;  // R2
  tmpVar += m_hp_r2_b1 * m_hp_r2_stateVar_1;
  tmpVar += m_hp_r2_b0 * m_hp_r2_stateVar_2;
  tmpVar += m_hp_r2_a1 * m_hp_r2_stateVar_3;
  tmpVar += m_hp_r2_a2 * m_hp_r2_stateVar_4;

  m_hp_r2_stateVar_2 = m_hp_r2_stateVar_1 + NlToolbox::Constants::DNC_const;
  m_hp_r2_stateVar_1 = hp_sample + NlToolbox::Constants::DNC_const;
  m_hp_r2_stateVar_4 = m_hp_r2_stateVar_3 + NlToolbox::Constants::DNC_const;
  m_hp_r2_stateVar_3 = tmpVar + NlToolbox::Constants::DNC_const;

  hp_sample = tmpVar;

  //****************************** Lowpass R ******************************//
  tmpVar = hp_sample * signals.get<Signals::GAP_HPLP>() + _rawSample_R * signals.get<Signals::GAP_INLP>();

  lp_sample = m_lp_r1_b0 * tmpVar;  // R1
  lp_sample += m_lp_r1_b1 * m_lp_r1_stateVar_1;
  lp_sample += m_lp_r1_b0 * m_lp_r1_stateVar_2;
  lp_sample += m_lp_r1_a1 * m_lp_r1_stateVar_3;
  lp_sample += m_lp_r1_a2 * m_lp_r1_stateVar_4;

  m_lp_r1_stateVar_2 = m_lp_r1_stateVar_1 + NlToolbox::Constants::DNC_const;
  m_lp_r1_stateVar_1 = tmpVar + NlToolbox::Constants::DNC_const;
  m_lp_r1_stateVar_4 = m_lp_r1_stateVar_3 + NlToolbox::Constants::DNC_const;
  m_lp_r1_stateVar_3 = lp_sample + NlToolbox::Constants::DNC_const;

  tmpVar = m_lp_r2_b0 * lp_sample;  // R2
  tmpVar += m_lp_r2_b1 * m_lp_r2_stateVar_1;
  tmpVar += m_lp_r2_b0 * m_lp_r2_stateVar_2;
  tmpVar += m_lp_r2_a1 * m_lp_r2_stateVar_3;
  tmpVar += m_lp_r2_a2 * m_lp_r2_stateVar_4;

  m_lp_r2_stateVar_2 = m_lp_r2_stateVar_1 + NlToolbox::Constants::DNC_const;
  m_lp_r2_stateVar_1 = lp_sample + NlToolbox::Constants::DNC_const;
  m_lp_r2_stateVar_4 = m_lp_r2_stateVar_3 + NlToolbox::Constants::DNC_const;
  m_lp_r2_stateVar_3 = tmpVar + NlToolbox::Constants::DNC_const;

  lp_sample = tmpVar * signals.get<Signals::GAP_LPOUT>();
  hp_sample *= signals.get<Signals::GAP_HPOUT>();
  m_out_R = hp_sample + lp_sample + (_rawSample_R * signals.get<Signals::GAP_INOUT>());
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_gapfilter::resetDSP()
{
  m_out_L = 0.f;
  m_out_R = 0.f;

  //************************** Biquad Highpass L/R **************************//

  m_hp_l1_stateVar_1 = 0.f;
  m_hp_l1_stateVar_2 = 0.f;
  m_hp_l1_stateVar_3 = 0.f;
  m_hp_l1_stateVar_4 = 0.f;

  m_hp_l2_stateVar_1 = 0.f;
  m_hp_l2_stateVar_2 = 0.f;
  m_hp_l2_stateVar_3 = 0.f;
  m_hp_l2_stateVar_4 = 0.f;

  m_hp_r1_stateVar_1 = 0.f;
  m_hp_r1_stateVar_2 = 0.f;
  m_hp_r1_stateVar_3 = 0.f;
  m_hp_r1_stateVar_4 = 0.f;

  m_hp_r2_stateVar_1 = 0.f;
  m_hp_r2_stateVar_2 = 0.f;
  m_hp_r2_stateVar_3 = 0.f;
  m_hp_r2_stateVar_4 = 0.f;

  //*************************** Biquad Lowpass L/R **************************//

  m_lp_l1_stateVar_1 = 0.f;
  m_lp_l1_stateVar_2 = 0.f;
  m_lp_l1_stateVar_3 = 0.f;
  m_lp_l1_stateVar_4 = 0.f;

  m_lp_l2_stateVar_1 = 0.f;
  m_lp_l2_stateVar_2 = 0.f;
  m_lp_l2_stateVar_3 = 0.f;
  m_lp_l2_stateVar_4 = 0.f;

  m_lp_r1_stateVar_1 = 0.f;
  m_lp_r1_stateVar_2 = 0.f;
  m_lp_r1_stateVar_3 = 0.f;
  m_lp_r1_stateVar_4 = 0.f;

  m_lp_r2_stateVar_1 = 0.f;
  m_lp_r2_stateVar_2 = 0.f;
  m_lp_r2_stateVar_3 = 0.f;
  m_lp_r2_stateVar_4 = 0.f;
}
