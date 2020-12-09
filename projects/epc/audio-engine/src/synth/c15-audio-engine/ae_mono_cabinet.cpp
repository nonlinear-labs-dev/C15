#include "ae_mono_cabinet.h"

/******************************************************************************/
/** @file       ae_mono_cabinet.cpp
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      ...
    @todo
*******************************************************************************/

Engine::MonoCabinet::MonoCabinet()
{
}

void Engine::MonoCabinet::init(const float _samplerate)
{
  // outputs
  m_out_L = 0.0f;
  m_out_R = 0.0f;
  // prewarp
  m_warpConst_2PI = NlToolbox::Constants::twopi / _samplerate;
  m_freqClip_max = _samplerate / 2.125f;
  m_freqClip_min = _samplerate / 24576.0f;
  // biquad hp
  m_hp_b0 = 0.0f;
  m_hp_b1 = 0.0f;
  m_hp_a1 = 0.0f;
  m_hp_a2 = 0.0f;
  m_hp_stateVar_L1 = 0.0f;
  m_hp_stateVar_L2 = 0.0f;
  m_hp_stateVar_L3 = 0.0f;
  m_hp_stateVar_L4 = 0.0f;
  m_hp_stateVar_R1 = 0.0f;
  m_hp_stateVar_R2 = 0.0f;
  m_hp_stateVar_R3 = 0.0f;
  m_hp_stateVar_R4 = 0.0f;
  // biquad lp
  m_lp1_b0 = 0.0f;
  m_lp1_b1 = 0.0f;
  m_lp1_a1 = 0.0f;
  m_lp1_a2 = 0.0f;
  m_lp1_stateVar_L1 = 0.0f;
  m_lp1_stateVar_L2 = 0.0f;
  m_lp1_stateVar_L3 = 0.0f;
  m_lp1_stateVar_L4 = 0.0f;
  m_lp1_stateVar_R1 = 0.0f;
  m_lp1_stateVar_R2 = 0.0f;
  m_lp1_stateVar_R3 = 0.0f;
  m_lp1_stateVar_R4 = 0.0f;
  m_lp2_b0 = 0.0f;
  m_lp2_b1 = 0.0f;
  m_lp2_a1 = 0.0f;
  m_lp2_a2 = 0.0f;
  m_lp2_stateVar_L1 = 0.0f;
  m_lp2_stateVar_R1 = 0.0f;
  m_lp2_stateVar_L2 = 0.0f;
  m_lp2_stateVar_R2 = 0.0f;
  m_lp2_stateVar_L3 = 0.0f;
  m_lp2_stateVar_R3 = 0.0f;
  m_lp2_stateVar_L4 = 0.0f;
  m_lp2_stateVar_R4 = 0.0f;
  // tilt low shelves
  m_tiltOmegaSin = NlToolbox::Math::sin(1200.0f * m_warpConst_2PI);
  m_tiltOmegaCos = NlToolbox::Math::cos(1200.0f * m_warpConst_2PI);
  m_ls1_b0 = 0.0f;
  m_ls1_b1 = 0.0f;
  m_ls1_b2 = 0.0f;
  m_ls1_a1 = 0.0f;
  m_ls1_a2 = 0.0f;
  m_ls1_stateVar_L1 = 0.0f;
  m_ls1_StateVar_L2 = 0.0f;
  m_ls1_StateVar_L3 = 0.0f;
  m_ls1_StateVar_L4 = 0.0f;
  m_ls1_stateVar_R1 = 0.0f;
  m_ls1_stateVar_R2 = 0.0f;
  m_ls1_stateVar_R3 = 0.0f;
  m_ls1_stateVar_R4 = 0.0f;
  m_ls2_b0 = 0.0f;
  m_ls2_b1 = 0.0f;
  m_ls2_b2 = 0.0f;
  m_ls2_a1 = 0.0f;
  m_ls2_a2 = 0.0f;
  m_ls2_stateVar_L1 = 0.0f;
  m_ls2_StateVar_L2 = 0.0f;
  m_ls2_StateVar_L3 = 0.0f;
  m_ls2_stateVar_R1 = 0.0f;
  m_ls2_StateVar_L4 = 0.0f;
  m_ls2_stateVar_R2 = 0.0f;
  m_ls2_stateVar_R3 = 0.0f;
  m_ls2_stateVar_R4 = 0.0f;
  // 30 Hz highpass
  m_hp30_b0 = 30.0f * m_warpConst_2PI;
  m_hp30_b0 = std::min(m_hp30_b0, 0.8f);
  m_hp30_stateVar_L = 0.0f;
  m_hp30_stateVar_R = 0.0f;
}

void Engine::MonoCabinet::set(MonoSignals &_signals)
{
  float tmpVar;
  // biquad hp
  float frequency = _signals.get(C15::Signals::Mono_Signals::Cabinet_HPF);
  frequency = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  frequency *= m_warpConst_2PI;
  tmpVar = NlToolbox::Math::cos(frequency);
  m_hp_a1 = tmpVar * -2.0f;
  tmpVar += 1.0f;
  m_hp_b0 = tmpVar * 0.5f;
  m_hp_b1 = -tmpVar;
  tmpVar = NlToolbox::Math::sin(frequency) * 0.5f;
  m_hp_a2 = 1.0f - tmpVar;
  tmpVar = 1.0f / (1.0f + tmpVar);
  m_hp_a1 = m_hp_a1 * -tmpVar;
  m_hp_a2 = m_hp_a2 * -tmpVar;
  m_hp_b0 = m_hp_b0 * tmpVar;
  m_hp_b1 = m_hp_b1 * tmpVar;
  // biquad lp 1
  frequency = _signals.get(C15::Signals::Mono_Signals::Cabinet_LPF);
  frequency = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  frequency *= m_warpConst_2PI;
  tmpVar = NlToolbox::Math::cos(frequency);
  m_lp1_a1 = tmpVar * -2.0f;
  tmpVar = 1.0f - tmpVar;
  m_lp1_b0 = tmpVar * 0.5f;
  m_lp1_b1 = tmpVar;
  tmpVar = NlToolbox::Math::sin(frequency) * 0.5f;
  m_lp1_a2 = 1.0f - tmpVar;
  tmpVar = 1.0f / (1.0f + tmpVar);
  m_lp1_a1 = m_lp1_a1 * -tmpVar;
  m_lp1_a2 = m_lp1_a2 * -tmpVar;
  m_lp1_b0 = m_lp1_b0 * tmpVar;
  m_lp1_b1 = m_lp1_b1 * tmpVar;
  // biquad lp 2
  frequency = _signals.get(C15::Signals::Mono_Signals::Cabinet_LPF) * 1.333f;
  frequency = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  frequency *= m_warpConst_2PI;
  tmpVar = NlToolbox::Math::cos(frequency);
  m_lp2_a1 = tmpVar * -2.0f;
  tmpVar = 1.0f - tmpVar;
  m_lp2_b0 = tmpVar * 0.5f;
  m_lp2_b1 = tmpVar;
  tmpVar = NlToolbox::Math::sin(frequency) * 0.5f;
  m_lp2_a2 = 1.0f - tmpVar;
  tmpVar = 1.0f / (1.0f + tmpVar);
  m_lp2_a1 = m_lp2_a1 * -tmpVar;
  m_lp2_a2 = m_lp2_a2 * -tmpVar;
  m_lp2_b0 = m_lp2_b0 * tmpVar;
  m_lp2_b1 = m_lp2_b1 * tmpVar;
  // tilt low shelves
  float tilt = std::pow(10.0f, _signals.get(C15::Signals::Mono_Signals::Cabinet_Tilt) * 0.025f);
  tmpVar = tilt + (1.0f / tilt) + 2.0f;
  tmpVar = std::sqrt(tilt * tmpVar) * m_tiltOmegaSin;
  m_ls1_a1 = ((tilt - 1.0f) + (m_tiltOmegaCos * (tilt + 1.0f))) * -2.0f;
  m_ls1_a2 = (tilt + 1.0f) + (m_tiltOmegaCos * (tilt - 1.0f)) - tmpVar;
  m_ls1_b0 = ((tilt + 1.0f) - (m_tiltOmegaCos * (tilt - 1.0f)) + tmpVar) * tilt;
  m_ls1_b1 = ((tilt - 1.0f) - (m_tiltOmegaCos * (tilt + 1.0f))) * 2.0f * tilt;
  m_ls1_b2 = ((tilt + 1.0f) - (m_tiltOmegaCos * (tilt - 1.0f)) - tmpVar) * tilt;
  float coeff = 1.0f / ((tilt + 1.0f) + (m_tiltOmegaCos * (tilt - 1.0f)) + tmpVar);
  m_ls1_a1 = m_ls1_a1 * -coeff;
  m_ls1_a2 = m_ls1_a2 * -coeff;
  m_ls1_b0 = m_ls1_b0 * coeff;
  m_ls1_b1 = m_ls1_b1 * coeff;
  m_ls1_b2 = m_ls1_b2 * coeff;
  tilt = std::pow(10.0f, _signals.get(C15::Signals::Mono_Signals::Cabinet_Tilt) * -0.025f);
  tmpVar = tilt + (1.0f / tilt) + 2.0f;
  tmpVar = std::sqrt(tilt * tmpVar) * m_tiltOmegaSin;
  m_ls2_a1 = ((tilt - 1.0f) + (m_tiltOmegaCos * (tilt + 1.0f))) * -2.0f;
  m_ls2_a2 = (tilt + 1.0f) + (m_tiltOmegaCos * (tilt - 1.0f)) - tmpVar;
  m_ls2_b0 = ((tilt + 1.0f) - (m_tiltOmegaCos * (tilt - 1.0f)) + tmpVar) * tilt;
  m_ls2_b1 = ((tilt - 1.0f) - (m_tiltOmegaCos * (tilt + 1.0f))) * 2.0f * tilt;
  m_ls2_b2 = ((tilt + 1.0f) - (m_tiltOmegaCos * (tilt - 1.0f)) - tmpVar) * tilt;
  coeff = 1.0f / ((tilt + 1.0f) + (m_tiltOmegaCos * (tilt - 1.0f)) + tmpVar);
  m_ls2_a1 = m_ls2_a1 * -coeff;
  m_ls2_a2 = m_ls2_a2 * -coeff;
  m_ls2_b0 = m_ls2_b0 * coeff;
  m_ls2_b1 = m_ls2_b1 * coeff;
  m_ls2_b2 = m_ls2_b2 * coeff;
}

void Engine::MonoCabinet::apply(MonoSignals &_signals, const float _rawSample_L, const float _rawSample_R)
{
  float tmpVar;
  // drive
  float sample_L = _rawSample_L * _signals.get(C15::Signals::Mono_Signals::Cabinet_Drive);
  float sample_R = _rawSample_R * _signals.get(C15::Signals::Mono_Signals::Cabinet_Drive);
  // biquad hp l
  tmpVar = m_hp_b0 * sample_L;
  tmpVar += m_hp_b1 * m_hp_stateVar_L1;
  tmpVar += m_hp_b0 * m_hp_stateVar_L2;
  tmpVar += m_hp_a1 * m_hp_stateVar_L3;
  tmpVar += m_hp_a2 * m_hp_stateVar_L4;
  m_hp_stateVar_L2 = m_hp_stateVar_L1;
  m_hp_stateVar_L1 = sample_L;
  m_hp_stateVar_L4 = m_hp_stateVar_L3;
  m_hp_stateVar_L3 = tmpVar;
  sample_L = tmpVar;
  // biquad hp r
  tmpVar = m_hp_b0 * sample_R;
  tmpVar += m_hp_b1 * m_hp_stateVar_R1;
  tmpVar += m_hp_b0 * m_hp_stateVar_R2;
  tmpVar += m_hp_a1 * m_hp_stateVar_R3;
  tmpVar += m_hp_a2 * m_hp_stateVar_R4;
  m_hp_stateVar_R2 = m_hp_stateVar_R1;
  m_hp_stateVar_R1 = sample_R;
  m_hp_stateVar_R4 = m_hp_stateVar_R3;
  m_hp_stateVar_R3 = tmpVar;
  sample_R = tmpVar;
  // tilt lowshelf l1
  tmpVar = m_ls1_b0 * sample_L;
  tmpVar += m_ls1_b1 * m_ls1_stateVar_L1;
  tmpVar += m_ls1_b2 * m_ls1_StateVar_L2;
  tmpVar += m_ls1_a1 * m_ls1_StateVar_L3;
  tmpVar += m_ls1_a2 * m_ls1_StateVar_L4;
  m_ls1_StateVar_L2 = m_ls1_stateVar_L1;
  m_ls1_stateVar_L1 = sample_L;
  m_ls1_StateVar_L4 = m_ls1_StateVar_L3;
  m_ls1_StateVar_L3 = tmpVar;
  sample_L = tmpVar;
  // tilt lowshelf r1
  tmpVar = m_ls1_b0 * sample_R;
  tmpVar += m_ls1_b1 * m_ls1_stateVar_R1;
  tmpVar += m_ls1_b2 * m_ls1_stateVar_R2;
  tmpVar += m_ls1_a1 * m_ls1_stateVar_R3;
  tmpVar += m_ls1_a2 * m_ls1_stateVar_R4;
  m_ls1_stateVar_R2 = m_ls1_stateVar_R1;
  m_ls1_stateVar_R1 = sample_R;
  m_ls1_stateVar_R4 = m_ls1_stateVar_R3;
  m_ls1_stateVar_R3 = tmpVar;
  sample_R = tmpVar;
  // shaper l
  sample_L *= _signals.get(C15::Signals::Mono_Signals::Cabinet_Pre_Sat);
  tmpVar = sample_L;
  sample_L = NlToolbox::Math::sinP3_wrap(sample_L);
  sample_L = NlToolbox::Others::threeRanges(sample_L, tmpVar, _signals.get(C15::Signals::Mono_Signals::Cabinet_Fold));
  tmpVar = sample_L * sample_L - m_hp30_stateVar_L;
  m_hp30_stateVar_L = tmpVar * m_hp30_b0 + m_hp30_stateVar_L;
  sample_L = NlToolbox::Others::parAsym(sample_L, tmpVar, _signals.get(C15::Signals::Mono_Signals::Cabinet_Asym));
  sample_L *= _signals.get(C15::Signals::Mono_Signals::Cabinet_Sat);
  // shaper r
  sample_R *= _signals.get(C15::Signals::Mono_Signals::Cabinet_Pre_Sat);
  tmpVar = sample_R;
  sample_R = NlToolbox::Math::sinP3_wrap(sample_R);
  sample_R = NlToolbox::Others::threeRanges(sample_R, tmpVar, _signals.get(C15::Signals::Mono_Signals::Cabinet_Fold));
  tmpVar = sample_R * sample_R - m_hp30_stateVar_R;
  m_hp30_stateVar_R = tmpVar * m_hp30_b0 + m_hp30_stateVar_R;
  sample_R = NlToolbox::Others::parAsym(sample_R, tmpVar, _signals.get(C15::Signals::Mono_Signals::Cabinet_Asym));
  sample_R *= _signals.get(C15::Signals::Mono_Signals::Cabinet_Sat);
  // tilt lowshelf l2
  tmpVar = m_ls2_b0 * sample_L;
  tmpVar += m_ls2_b1 * m_ls2_stateVar_L1;
  tmpVar += m_ls2_b2 * m_ls2_StateVar_L2;
  tmpVar += m_ls2_a1 * m_ls2_StateVar_L3;
  tmpVar += m_ls2_a2 * m_ls2_StateVar_L4;
  m_ls2_StateVar_L2 = m_ls2_stateVar_L1;
  m_ls2_stateVar_L1 = sample_L;
  m_ls2_StateVar_L4 = m_ls2_StateVar_L3;
  m_ls2_StateVar_L3 = tmpVar;
  sample_L = tmpVar;
  // tilt lowshelf r2
  tmpVar = m_ls2_b0 * sample_R;
  tmpVar += m_ls2_b1 * m_ls2_stateVar_R1;
  tmpVar += m_ls2_b2 * m_ls2_stateVar_R2;
  tmpVar += m_ls2_a1 * m_ls2_stateVar_R3;
  tmpVar += m_ls2_a2 * m_ls2_stateVar_R4;
  m_ls2_stateVar_R2 = m_ls2_stateVar_R1;
  m_ls2_stateVar_R1 = sample_R;
  m_ls2_stateVar_R4 = m_ls2_stateVar_R3;
  m_ls2_stateVar_R3 = tmpVar;
  sample_R = tmpVar;
  // 2x biquad lp l
  tmpVar = m_lp1_b0 * sample_L;
  tmpVar += m_lp1_b1 * m_lp1_stateVar_L1;
  tmpVar += m_lp1_b0 * m_lp1_stateVar_L2;
  tmpVar += m_lp1_a1 * m_lp1_stateVar_L3;
  tmpVar += m_lp1_a2 * m_lp1_stateVar_L4;
  m_lp1_stateVar_L2 = m_lp1_stateVar_L1;
  m_lp1_stateVar_L1 = sample_L;
  m_lp1_stateVar_L4 = m_lp1_stateVar_L3;
  m_lp1_stateVar_L3 = tmpVar;
  sample_L = tmpVar;
  tmpVar = m_lp2_b0 * sample_L;
  tmpVar += m_lp2_b1 * m_lp2_stateVar_L1;
  tmpVar += m_lp2_b0 * m_lp2_stateVar_L2;
  tmpVar += m_lp2_a1 * m_lp2_stateVar_L3;
  tmpVar += m_lp2_a2 * m_lp2_stateVar_L4;
  m_lp2_stateVar_L2 = m_lp2_stateVar_L1;
  m_lp2_stateVar_L1 = sample_L;
  m_lp2_stateVar_L4 = m_lp2_stateVar_L3;
  m_lp2_stateVar_L3 = tmpVar;
  sample_L = tmpVar;
  // 2x biquad lp r
  tmpVar = m_lp1_b0 * sample_R;
  tmpVar += m_lp1_b1 * m_lp1_stateVar_R1;
  tmpVar += m_lp1_b0 * m_lp1_stateVar_R2;
  tmpVar += m_lp1_a1 * m_lp1_stateVar_R3;
  tmpVar += m_lp1_a2 * m_lp1_stateVar_R4;
  m_lp1_stateVar_R2 = m_lp1_stateVar_R1;
  m_lp1_stateVar_R1 = sample_R;
  m_lp1_stateVar_R4 = m_lp1_stateVar_R3;
  m_lp1_stateVar_R3 = tmpVar;
  sample_R = tmpVar;
  tmpVar = m_lp2_b0 * sample_R;
  tmpVar += m_lp2_b1 * m_lp2_stateVar_R1;
  tmpVar += m_lp2_b0 * m_lp2_stateVar_R2;
  tmpVar += m_lp2_a1 * m_lp2_stateVar_R3;
  tmpVar += m_lp2_a2 * m_lp2_stateVar_R4;
  m_lp2_stateVar_R2 = m_lp2_stateVar_R1;
  m_lp2_stateVar_R1 = sample_R;
  m_lp2_stateVar_R4 = m_lp2_stateVar_R3;
  m_lp2_stateVar_R3 = tmpVar;
  sample_R = tmpVar;
  // crossfades
  m_out_L
      = NlToolbox::Crossfades::crossFade(_rawSample_L, sample_L, _signals.get(C15::Signals::Mono_Signals::Cabinet_Dry),
                                         _signals.get(C15::Signals::Mono_Signals::Cabinet_Wet));
  m_out_R
      = NlToolbox::Crossfades::crossFade(_rawSample_R, sample_R, _signals.get(C15::Signals::Mono_Signals::Cabinet_Dry),
                                         _signals.get(C15::Signals::Mono_Signals::Cabinet_Wet));
}

void Engine::MonoCabinet::resetDSP()
{
  // outputs
  m_out_L = 0.0f;
  m_out_R = 0.0f;
  // biquad hp
  m_hp_stateVar_L1 = 0.0f;
  m_hp_stateVar_L2 = 0.0f;
  m_hp_stateVar_L3 = 0.0f;
  m_hp_stateVar_L4 = 0.0f;
  m_hp_stateVar_R1 = 0.0f;
  m_hp_stateVar_R2 = 0.0f;
  m_hp_stateVar_R3 = 0.0f;
  m_hp_stateVar_R4 = 0.0f;
  // biquad lp
  m_lp1_stateVar_L1 = 0.0f;
  m_lp1_stateVar_L2 = 0.0f;
  m_lp1_stateVar_L3 = 0.0f;
  m_lp1_stateVar_L4 = 0.0f;
  m_lp1_stateVar_R1 = 0.0f;
  m_lp1_stateVar_R2 = 0.0f;
  m_lp1_stateVar_R3 = 0.0f;
  m_lp1_stateVar_R4 = 0.0f;
  m_lp2_stateVar_L1 = 0.0f;
  m_lp2_stateVar_L2 = 0.0f;
  m_lp2_stateVar_L3 = 0.0f;
  m_lp2_stateVar_L4 = 0.0f;
  m_lp2_stateVar_R1 = 0.0f;
  m_lp2_stateVar_R2 = 0.0f;
  m_lp2_stateVar_R3 = 0.0f;
  m_lp2_stateVar_R4 = 0.0f;
  // tilt low shelves
  m_ls1_stateVar_L1 = 0.0f;
  m_ls1_StateVar_L2 = 0.0f;
  m_ls1_StateVar_L3 = 0.0f;
  m_ls1_StateVar_L4 = 0.0f;
  m_ls1_stateVar_R1 = 0.0f;
  m_ls1_stateVar_R2 = 0.0f;
  m_ls1_stateVar_R3 = 0.0f;
  m_ls1_stateVar_R4 = 0.0f;
  m_ls2_stateVar_L1 = 0.0f;
  m_ls2_StateVar_L2 = 0.0f;
  m_ls2_StateVar_L3 = 0.0f;
  m_ls2_StateVar_L4 = 0.0f;
  m_ls2_stateVar_R1 = 0.0f;
  m_ls2_stateVar_R2 = 0.0f;
  m_ls2_stateVar_R3 = 0.0f;
  m_ls2_stateVar_R4 = 0.0f;
  // 30 Hz hp
  m_hp30_stateVar_L = 0.0f;
  m_hp30_stateVar_R = 0.0f;
}
