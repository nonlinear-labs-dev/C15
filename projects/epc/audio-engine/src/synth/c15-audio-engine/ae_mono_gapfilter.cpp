#include "ae_mono_gapfilter.h"

/******************************************************************************/
/** @file       ae_mono_gapfilter.cpp
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      ...
    @todo
*******************************************************************************/

Engine::MonoGapFilter::MonoGapFilter()
{
}

void Engine::MonoGapFilter::init(const float _samplerate)
{
  // output
  m_out_L = 0.0f;
  m_out_R = 0.0f;
  // prewarp
  m_warpConst_2PI = NlToolbox::Constants::twopi / _samplerate;
  m_freqClip_max = _samplerate / 2.125f;
  m_freqClip_min = _samplerate / 24576.0f;
  m_freqScale_max = _samplerate / 3.0f;
  m_freqScale_min = _samplerate / 2.0f;
  m_freqScale_norm = 1.0f / (m_freqScale_max - m_freqScale_min);
  // biquad hp l/r
  m_hp_l1_b0 = 0.0f;
  m_hp_l1_b1 = 0.0f;
  m_hp_l1_a1 = 0.0f;
  m_hp_l1_a2 = 0.0f;
  m_hp_l2_b0 = 0.0f;
  m_hp_l2_b1 = 0.0f;
  m_hp_l2_a1 = 0.0f;
  m_hp_l2_a2 = 0.0f;
  m_hp_l1_stateVar_1 = 0.0f;
  m_hp_l1_stateVar_2 = 0.0f;
  m_hp_l1_stateVar_3 = 0.0f;
  m_hp_l1_stateVar_4 = 0.0f;
  m_hp_l2_stateVar_1 = 0.0f;
  m_hp_l2_stateVar_2 = 0.0f;
  m_hp_l2_stateVar_3 = 0.0f;
  m_hp_l2_stateVar_4 = 0.0f;
  m_hp_r1_b0 = 0.0f;
  m_hp_r1_b1 = 0.0f;
  m_hp_r1_a1 = 0.0f;
  m_hp_r1_a2 = 0.0f;
  m_hp_r2_b0 = 0.0f;
  m_hp_r2_b1 = 0.0f;
  m_hp_r2_a1 = 0.0f;
  m_hp_r2_a2 = 0.0f;
  m_hp_r1_stateVar_1 = 0.0f;
  m_hp_r1_stateVar_2 = 0.0f;
  m_hp_r1_stateVar_3 = 0.0f;
  m_hp_r1_stateVar_4 = 0.0f;
  m_hp_r2_stateVar_1 = 0.0f;
  m_hp_r2_stateVar_2 = 0.0f;
  m_hp_r2_stateVar_3 = 0.0f;
  m_hp_r2_stateVar_4 = 0.0f;
  // biquad lp l/r
  m_lp_l1_b0 = 0.0f;
  m_lp_l1_b1 = 0.0f;
  m_lp_l1_a1 = 0.0f;
  m_lp_l1_a2 = 0.0f;
  m_lp_l2_b0 = 0.0f;
  m_lp_l2_b1 = 0.0f;
  m_lp_l2_a1 = 0.0f;
  m_lp_l2_a2 = 0.0f;
  m_lp_l1_stateVar_1 = 0.0f;
  m_lp_l1_stateVar_2 = 0.0f;
  m_lp_l1_stateVar_3 = 0.0f;
  m_lp_l1_stateVar_4 = 0.0f;
  m_lp_l2_stateVar_1 = 0.0f;
  m_lp_l2_stateVar_2 = 0.0f;
  m_lp_l2_stateVar_3 = 0.0f;
  m_lp_l2_stateVar_4 = 0.0f;
  m_lp_r1_b0 = 0.0f;
  m_lp_r1_b1 = 0.0f;
  m_lp_r1_a1 = 0.0f;
  m_lp_r1_a2 = 0.0f;
  m_lp_r2_b0 = 0.0f;
  m_lp_r2_b1 = 0.0f;
  m_lp_r2_a1 = 0.0f;
  m_lp_r2_a2 = 0.0f;
  m_lp_r1_stateVar_1 = 0.0f;
  m_lp_r1_stateVar_2 = 0.0f;
  m_lp_r1_stateVar_3 = 0.0f;
  m_lp_r1_stateVar_4 = 0.0f;
  m_lp_r2_stateVar_1 = 0.0f;
  m_lp_r2_stateVar_2 = 0.0f;
  m_lp_r2_stateVar_3 = 0.0f;
  m_lp_r2_stateVar_4 = 0.0f;
}

void Engine::MonoGapFilter::set(MonoSignals &_signals)
{
  float tmpVar_1, tmpVar_2;
  // biquad hp l
  float frequency = _signals.get(C15::Signals::Mono_Signals::Gap_Flt_HF_L);
  tmpVar_1 = (frequency - m_freqScale_min) * m_freqScale_norm;
  tmpVar_1 = std::clamp(tmpVar_1, -1.0f, 1.0f);
  float resonance = _signals.get(C15::Signals::Mono_Signals::Gap_Flt_Res) * tmpVar_1;
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // L1
  tmpVar_1 *= m_warpConst_2PI;
  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_hp_l1_a1 = tmpVar_2 * -2.0f;
  tmpVar_2 += 1.0f;
  m_hp_l1_b0 = tmpVar_2 * 0.5f;
  m_hp_l1_b1 = -tmpVar_2;
  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.0f - resonance);
  m_hp_l1_a2 = 1.0f - tmpVar_2;
  tmpVar_2 = 1.0f / (1.0f + tmpVar_2);
  m_hp_l1_a1 = m_hp_l1_a1 * -tmpVar_2;
  m_hp_l1_a2 = m_hp_l1_a2 * -tmpVar_2;
  m_hp_l1_b0 = m_hp_l1_b0 * tmpVar_2;
  m_hp_l1_b1 = m_hp_l1_b1 * tmpVar_2;
  frequency *= 0.75f;  // L2
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  tmpVar_1 *= m_warpConst_2PI;
  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_hp_l2_a1 = tmpVar_2 * -2.0f;
  tmpVar_2 += 1.0f;
  m_hp_l2_b0 = tmpVar_2 * 0.5f;
  m_hp_l2_b1 = -tmpVar_2;
  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.0f - resonance);
  m_hp_l2_a2 = 1.0f - tmpVar_2;
  tmpVar_2 = 1.0f / (1.0f + tmpVar_2);
  m_hp_l2_a1 = m_hp_l2_a1 * -tmpVar_2;
  m_hp_l2_a2 = m_hp_l2_a2 * -tmpVar_2;
  m_hp_l2_b0 = m_hp_l2_b0 * tmpVar_2;
  m_hp_l2_b1 = m_hp_l2_b1 * tmpVar_2;
  // biquad hp r
  frequency = _signals.get(C15::Signals::Mono_Signals::Gap_Flt_HF_R);
  tmpVar_1 = (frequency - m_freqScale_min) * m_freqScale_norm;
  tmpVar_1 = std::clamp(tmpVar_1, -1.0f, 1.0f);
  resonance = _signals.get(C15::Signals::Mono_Signals::Gap_Flt_Res) * tmpVar_1;
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // R1
  tmpVar_1 *= m_warpConst_2PI;
  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_hp_r1_a1 = tmpVar_2 * -2.0f;
  tmpVar_2 += 1.0f;
  m_hp_r1_b0 = tmpVar_2 * 0.5f;
  m_hp_r1_b1 = -tmpVar_2;
  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.0f - resonance);
  m_hp_r1_a2 = 1.0f - tmpVar_2;
  tmpVar_2 = 1.0f / (1.0f + tmpVar_2);
  m_hp_r1_a1 = m_hp_r1_a1 * -tmpVar_2;
  m_hp_r1_a2 = m_hp_r1_a2 * -tmpVar_2;
  m_hp_r1_b0 = m_hp_r1_b0 * tmpVar_2;
  m_hp_r1_b1 = m_hp_r1_b1 * tmpVar_2;
  frequency *= 0.75f;  // R2
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  tmpVar_1 *= m_warpConst_2PI;
  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_hp_r2_a1 = tmpVar_2 * -2.0f;
  tmpVar_2 += 1.0f;
  m_hp_r2_b0 = tmpVar_2 * 0.5f;
  m_hp_r2_b1 = -tmpVar_2;
  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.0f - resonance);
  m_hp_r2_a2 = 1.0f - tmpVar_2;
  tmpVar_2 = 1.0f / (1.0f + tmpVar_2);
  m_hp_r2_a1 = m_hp_r2_a1 * -tmpVar_2;
  m_hp_r2_a2 = m_hp_r2_a2 * -tmpVar_2;
  m_hp_r2_b0 = m_hp_r2_b0 * tmpVar_2;
  m_hp_r2_b1 = m_hp_r2_b1 * tmpVar_2;
  // biquad lp l
  frequency = _signals.get(C15::Signals::Mono_Signals::Gap_Flt_LF_L);
  tmpVar_1 = (frequency - m_freqScale_min) * m_freqScale_norm;
  tmpVar_1 = std::clamp(tmpVar_1, -1.0f, 1.0f);
  resonance = _signals.get(C15::Signals::Mono_Signals::Gap_Flt_Res) * tmpVar_1;
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // L1
  tmpVar_1 *= m_warpConst_2PI;
  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_lp_l1_a1 = tmpVar_2 * -2.0f;
  tmpVar_2 = 1.0f - tmpVar_2;
  m_lp_l1_b0 = tmpVar_2 * 0.5f;
  m_lp_l1_b1 = tmpVar_2;
  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.0f - resonance);
  m_lp_l1_a2 = 1.0f - tmpVar_2;
  tmpVar_2 = 1.0f / (1.0f + tmpVar_2);
  m_lp_l1_a1 = m_lp_l1_a1 * -tmpVar_2;
  m_lp_l1_a2 = m_lp_l1_a2 * -tmpVar_2;
  m_lp_l1_b0 = m_lp_l1_b0 * tmpVar_2;
  m_lp_l1_b1 = m_lp_l1_b1 * tmpVar_2;
  frequency *= 1.33f;
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // L2
  tmpVar_1 *= m_warpConst_2PI;
  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_lp_l2_a1 = tmpVar_2 * -2.0f;
  tmpVar_2 = 1.0f - tmpVar_2;
  m_lp_l2_b0 = tmpVar_2 * 0.5f;
  m_lp_l2_b1 = tmpVar_2;
  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.0f - resonance);
  m_lp_l2_a2 = 1.0f - tmpVar_2;
  tmpVar_2 = 1.0f / (1.0f + tmpVar_2);
  m_lp_l2_a1 = m_lp_l2_a1 * -tmpVar_2;
  m_lp_l2_a2 = m_lp_l2_a2 * -tmpVar_2;
  m_lp_l2_b0 = m_lp_l2_b0 * tmpVar_2;
  m_lp_l2_b1 = m_lp_l2_b1 * tmpVar_2;
  // biquad lp r
  frequency = _signals.get(C15::Signals::Mono_Signals::Gap_Flt_LF_R);
  tmpVar_1 = (frequency - m_freqScale_min) * m_freqScale_norm;
  tmpVar_1 = std::clamp(tmpVar_1, -1.0f, 1.0f);
  resonance = _signals.get(C15::Signals::Mono_Signals::Gap_Flt_Res) * tmpVar_1;
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // R1
  tmpVar_1 *= m_warpConst_2PI;
  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_lp_r1_a1 = tmpVar_2 * -2.0f;
  tmpVar_2 = 1.0f - tmpVar_2;
  m_lp_r1_b0 = tmpVar_2 * 0.5f;
  m_lp_r1_b1 = tmpVar_2;
  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.0f - resonance);
  m_lp_r1_a2 = 1.0f - tmpVar_2;
  tmpVar_2 = 1.0f / (1.0f + tmpVar_2);
  m_lp_r1_a1 = m_lp_r1_a1 * -tmpVar_2;
  m_lp_r1_a2 = m_lp_r1_a2 * -tmpVar_2;
  m_lp_r1_b0 = m_lp_r1_b0 * tmpVar_2;
  m_lp_r1_b1 = m_lp_r1_b1 * tmpVar_2;
  frequency *= 1.33f;
  tmpVar_1 = std::clamp(frequency, m_freqClip_min, m_freqClip_max);  // R2
  tmpVar_1 *= m_warpConst_2PI;
  tmpVar_2 = NlToolbox::Math::cos(tmpVar_1);
  m_lp_r2_a1 = tmpVar_2 * -2.0f;
  tmpVar_2 = 1.0f - tmpVar_2;
  m_lp_r2_b0 = tmpVar_2 * 0.5f;
  m_lp_r2_b1 = tmpVar_2;
  tmpVar_2 = NlToolbox::Math::sin(tmpVar_1) * (1.0f - resonance);
  m_lp_r2_a2 = 1.0f - tmpVar_2;
  tmpVar_2 = 1.0f / (1.0f + tmpVar_2);
  m_lp_r2_a1 = m_lp_r2_a1 * -tmpVar_2;
  m_lp_r2_a2 = m_lp_r2_a2 * -tmpVar_2;
  m_lp_r2_b0 = m_lp_r2_b0 * tmpVar_2;
  m_lp_r2_b1 = m_lp_r2_b1 * tmpVar_2;
}

void Engine::MonoGapFilter::apply(MonoSignals &_signals, const float _rawSample_L, const float _rawSample_R)
{
  float tmpVar;
  // hp l
  float hp_sample = m_hp_l1_b0 * _rawSample_L;  // L1
  hp_sample += m_hp_l1_b1 * m_hp_l1_stateVar_1;
  hp_sample += m_hp_l1_b0 * m_hp_l1_stateVar_2;
  hp_sample += m_hp_l1_a1 * m_hp_l1_stateVar_3;
  hp_sample += m_hp_l1_a2 * m_hp_l1_stateVar_4;
  m_hp_l1_stateVar_2 = m_hp_l1_stateVar_1;
  m_hp_l1_stateVar_1 = _rawSample_L;
  m_hp_l1_stateVar_4 = m_hp_l1_stateVar_3;
  m_hp_l1_stateVar_3 = hp_sample;
  tmpVar = m_hp_l2_b0 * hp_sample;  // L2
  tmpVar += m_hp_l2_b1 * m_hp_l2_stateVar_1;
  tmpVar += m_hp_l2_b0 * m_hp_l2_stateVar_2;
  tmpVar += m_hp_l2_a1 * m_hp_l2_stateVar_3;
  tmpVar += m_hp_l2_a2 * m_hp_l2_stateVar_4;
  m_hp_l2_stateVar_2 = m_hp_l2_stateVar_1;
  m_hp_l2_stateVar_1 = hp_sample;
  m_hp_l2_stateVar_4 = m_hp_l2_stateVar_3;
  m_hp_l2_stateVar_3 = tmpVar;
  hp_sample = tmpVar;
  // lp l
  tmpVar = hp_sample * _signals.get(C15::Signals::Mono_Signals::Gap_Flt_HP_LP)
      + _rawSample_L * _signals.get(C15::Signals::Mono_Signals::Gap_Flt_In_LP);
  float lp_sample = m_lp_l1_b0 * tmpVar;  // L1
  lp_sample += m_lp_l1_b1 * m_lp_l1_stateVar_1;
  lp_sample += m_lp_l1_b0 * m_lp_l1_stateVar_2;
  lp_sample += m_lp_l1_a1 * m_lp_l1_stateVar_3;
  lp_sample += m_lp_l1_a2 * m_lp_l1_stateVar_4;
  m_lp_l1_stateVar_2 = m_lp_l1_stateVar_1;
  m_lp_l1_stateVar_1 = tmpVar;
  m_lp_l1_stateVar_4 = m_lp_l1_stateVar_3;
  m_lp_l1_stateVar_3 = lp_sample;
  tmpVar = m_lp_l2_b0 * lp_sample;  // L2
  tmpVar += m_lp_l2_b1 * m_lp_l2_stateVar_1;
  tmpVar += m_lp_l2_b0 * m_lp_l2_stateVar_2;
  tmpVar += m_lp_l2_a1 * m_lp_l2_stateVar_3;
  tmpVar += m_lp_l2_a2 * m_lp_l2_stateVar_4;
  m_lp_l2_stateVar_2 = m_lp_l2_stateVar_1;
  m_lp_l2_stateVar_1 = lp_sample;
  m_lp_l2_stateVar_4 = m_lp_l2_stateVar_3;
  m_lp_l2_stateVar_3 = tmpVar;
  lp_sample = tmpVar * _signals.get(C15::Signals::Mono_Signals::Gap_Flt_LP_Out);
  hp_sample *= _signals.get(C15::Signals::Mono_Signals::Gap_Flt_HP_Out);
  m_out_L = hp_sample + lp_sample + (_rawSample_L * _signals.get(C15::Signals::Mono_Signals::Gap_Flt_In_Out));
  // hp r
  hp_sample = m_hp_r1_b0 * _rawSample_R;  // R1
  hp_sample += m_hp_r1_b1 * m_hp_r1_stateVar_1;
  hp_sample += m_hp_r1_b0 * m_hp_r1_stateVar_2;
  hp_sample += m_hp_r1_a1 * m_hp_r1_stateVar_3;
  hp_sample += m_hp_r1_a2 * m_hp_r1_stateVar_4;
  m_hp_r1_stateVar_2 = m_hp_r1_stateVar_1;
  m_hp_r1_stateVar_1 = _rawSample_R;
  m_hp_r1_stateVar_4 = m_hp_r1_stateVar_3;
  m_hp_r1_stateVar_3 = hp_sample;
  tmpVar = m_hp_r2_b0 * hp_sample;  // R2
  tmpVar += m_hp_r2_b1 * m_hp_r2_stateVar_1;
  tmpVar += m_hp_r2_b0 * m_hp_r2_stateVar_2;
  tmpVar += m_hp_r2_a1 * m_hp_r2_stateVar_3;
  tmpVar += m_hp_r2_a2 * m_hp_r2_stateVar_4;
  m_hp_r2_stateVar_2 = m_hp_r2_stateVar_1;
  m_hp_r2_stateVar_1 = hp_sample;
  m_hp_r2_stateVar_4 = m_hp_r2_stateVar_3;
  m_hp_r2_stateVar_3 = tmpVar;
  hp_sample = tmpVar;
  // lp r
  tmpVar = hp_sample * _signals.get(C15::Signals::Mono_Signals::Gap_Flt_HP_LP)
      + _rawSample_R * _signals.get(C15::Signals::Mono_Signals::Gap_Flt_In_LP);
  lp_sample = m_lp_r1_b0 * tmpVar;  // R1
  lp_sample += m_lp_r1_b1 * m_lp_r1_stateVar_1;
  lp_sample += m_lp_r1_b0 * m_lp_r1_stateVar_2;
  lp_sample += m_lp_r1_a1 * m_lp_r1_stateVar_3;
  lp_sample += m_lp_r1_a2 * m_lp_r1_stateVar_4;
  m_lp_r1_stateVar_2 = m_lp_r1_stateVar_1;
  m_lp_r1_stateVar_1 = tmpVar;
  m_lp_r1_stateVar_4 = m_lp_r1_stateVar_3;
  m_lp_r1_stateVar_3 = lp_sample;
  tmpVar = m_lp_r2_b0 * lp_sample;  // R2
  tmpVar += m_lp_r2_b1 * m_lp_r2_stateVar_1;
  tmpVar += m_lp_r2_b0 * m_lp_r2_stateVar_2;
  tmpVar += m_lp_r2_a1 * m_lp_r2_stateVar_3;
  tmpVar += m_lp_r2_a2 * m_lp_r2_stateVar_4;
  m_lp_r2_stateVar_2 = m_lp_r2_stateVar_1;
  m_lp_r2_stateVar_1 = lp_sample;
  m_lp_r2_stateVar_4 = m_lp_r2_stateVar_3;
  m_lp_r2_stateVar_3 = tmpVar;
  lp_sample = tmpVar * _signals.get(C15::Signals::Mono_Signals::Gap_Flt_LP_Out);
  hp_sample *= _signals.get(C15::Signals::Mono_Signals::Gap_Flt_HP_Out);
  m_out_R = hp_sample + lp_sample + (_rawSample_R * _signals.get(C15::Signals::Mono_Signals::Gap_Flt_In_Out));
}

void Engine::MonoGapFilter::resetDSP()
{
  // outputs
  m_out_L = 0.0f;
  m_out_R = 0.0f;
  // biquad hp l/r
  m_hp_l1_stateVar_1 = 0.0f;
  m_hp_l1_stateVar_2 = 0.0f;
  m_hp_l1_stateVar_3 = 0.0f;
  m_hp_l1_stateVar_4 = 0.0f;
  m_hp_l2_stateVar_1 = 0.0f;
  m_hp_l2_stateVar_2 = 0.0f;
  m_hp_l2_stateVar_3 = 0.0f;
  m_hp_l2_stateVar_4 = 0.0f;
  m_hp_r1_stateVar_1 = 0.0f;
  m_hp_r1_stateVar_2 = 0.0f;
  m_hp_r1_stateVar_3 = 0.0f;
  m_hp_r1_stateVar_4 = 0.0f;
  m_hp_r2_stateVar_1 = 0.0f;
  m_hp_r2_stateVar_2 = 0.0f;
  m_hp_r2_stateVar_3 = 0.0f;
  m_hp_r2_stateVar_4 = 0.0f;
  // biquad lp l/r
  m_lp_l1_stateVar_1 = 0.0f;
  m_lp_l1_stateVar_2 = 0.0f;
  m_lp_l1_stateVar_3 = 0.0f;
  m_lp_l1_stateVar_4 = 0.0f;
  m_lp_l2_stateVar_1 = 0.0f;
  m_lp_l2_stateVar_2 = 0.0f;
  m_lp_l2_stateVar_3 = 0.0f;
  m_lp_l2_stateVar_4 = 0.0f;
  m_lp_r1_stateVar_1 = 0.0f;
  m_lp_r1_stateVar_2 = 0.0f;
  m_lp_r1_stateVar_3 = 0.0f;
  m_lp_r1_stateVar_4 = 0.0f;
  m_lp_r2_stateVar_1 = 0.0f;
  m_lp_r2_stateVar_2 = 0.0f;
  m_lp_r2_stateVar_3 = 0.0f;
  m_lp_r2_stateVar_4 = 0.0f;
}
