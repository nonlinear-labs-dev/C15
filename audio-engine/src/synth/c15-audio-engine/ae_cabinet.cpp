/******************************************************************************/
/** @file           ae_cabinet.cpp
    @date           2018-07-18
    @version        1.0
    @author         Anton Schmied
    @brief          Cabinet
    @todo
*******************************************************************************/

#include "ae_cabinet.h"
#include "SignalStorage.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_cabinet::ae_cabinet()
{
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_cabinet::init(float _samplerate)
{
  m_out_L = 0.f;
  m_out_R = 0.f;

  m_warpConst_2PI = 6.28319f / _samplerate;
  m_freqClip_max = _samplerate / 2.125f;
  m_freqClip_min = _samplerate / 24576.f;

  //*************************** Biquad Highpass ****************************//
  m_hp_b0 = 0.f;
  m_hp_b1 = 0.f;
  m_hp_a1 = 0.f;
  m_hp_a2 = 0.f;

  m_hp_stateVar_L1 = 0.f;
  m_hp_stateVar_L2 = 0.f;
  m_hp_stateVar_L3 = 0.f;
  m_hp_stateVar_L4 = 0.f;
  m_hp_stateVar_R1 = 0.f;
  m_hp_stateVar_R2 = 0.f;
  m_hp_stateVar_R3 = 0.f;
  m_hp_stateVar_R4 = 0.f;

  //************************** Biquad Lowpasses ****************************//
  m_lp1_b0 = 0.f;
  m_lp1_b1 = 0.f;
  m_lp1_a1 = 0.f;
  m_lp1_a2 = 0.f;

  m_lp1_stateVar_L1 = 0.f;
  m_lp1_stateVar_L2 = 0.f;
  m_lp1_stateVar_L3 = 0.f;
  m_lp1_stateVar_L4 = 0.f;
  m_lp1_stateVar_R1 = 0.f;
  m_lp1_stateVar_R2 = 0.f;
  m_lp1_stateVar_R3 = 0.f;
  m_lp1_stateVar_R4 = 0.f;

  m_lp2_b0 = 0.f;
  m_lp2_b1 = 0.f;
  m_lp2_a1 = 0.f;
  m_lp2_a2 = 0.f;

  m_lp2_stateVar_L1 = 0.f;
  m_lp2_stateVar_R1 = 0.f;
  m_lp2_stateVar_L2 = 0.f;
  m_lp2_stateVar_R2 = 0.f;
  m_lp2_stateVar_L3 = 0.f;
  m_lp2_stateVar_R3 = 0.f;
  m_lp2_stateVar_L4 = 0.f;
  m_lp2_stateVar_R4 = 0.f;

  //*************************** Tilt Lowshelves ****************************//
  m_tiltOmegaSin = NlToolbox::Math::sin(1200.f * m_warpConst_2PI);
  m_tiltOmegaCos = NlToolbox::Math::cos(1200.f * m_warpConst_2PI);

  m_ls1_b0 = 0.f;
  m_ls1_b1 = 0.f;
  m_ls1_b2 = 0.f;
  m_ls1_a1 = 0.f;
  m_ls1_a2 = 0.f;

  m_ls1_stateVar_L1 = 0.f;
  m_ls1_StateVar_L2 = 0.f;
  m_ls1_StateVar_L3 = 0.f;
  m_ls1_StateVar_L4 = 0.f;
  m_ls1_stateVar_R1 = 0.f;
  m_ls1_stateVar_R2 = 0.f;
  m_ls1_stateVar_R3 = 0.f;
  m_ls1_stateVar_R4 = 0.f;

  m_ls2_b0 = 0.f;
  m_ls2_b1 = 0.f;
  m_ls2_b2 = 0.f;
  m_ls2_a1 = 0.f;
  m_ls2_a2 = 0.f;

  m_ls2_stateVar_L1 = 0.f;
  m_ls2_StateVar_L2 = 0.f;
  m_ls2_StateVar_L3 = 0.f;
  m_ls2_stateVar_R1 = 0.f;
  m_ls2_StateVar_L4 = 0.f;
  m_ls2_stateVar_R2 = 0.f;
  m_ls2_stateVar_R3 = 0.f;
  m_ls2_stateVar_R4 = 0.f;

  //***************************** 30 Hz Highpass ****************************//
  m_hp30_b0 = 30.f * m_warpConst_2PI;
  m_hp30_b0 = std::min(m_hp30_b0, 0.8f);

  m_hp30_stateVar_L = 0.f;
  m_hp30_stateVar_R = 0.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_cabinet::set(SignalStorage &signals)
{
  float tmpVar;

  //*************************** Biquad Highpass ****************************//
  float frequency = signals.get<Signals::CAB_HPF>();
  frequency = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  frequency *= m_warpConst_2PI;

  tmpVar = NlToolbox::Math::cos(frequency);

  m_hp_a1 = tmpVar * -2.f;
  m_hp_b0 = (1.f + tmpVar) / 2.f;
  m_hp_b1 = (1.f + tmpVar) * -1.f;

  tmpVar = NlToolbox::Math::sin(frequency) * 0.5f;

  m_hp_a2 = 1.f - tmpVar;
  tmpVar = 1.f + tmpVar;

  m_hp_a1 = m_hp_a1 / tmpVar * -1.f;
  m_hp_a2 = m_hp_a2 / tmpVar * -1.f;
  m_hp_b0 = m_hp_b0 / tmpVar;
  m_hp_b1 = m_hp_b1 / tmpVar;

  //*************************** Biquad Lowpass 1 ***************************//
  frequency = signals.get<Signals::CAB_LPF>();
  frequency = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  frequency *= m_warpConst_2PI;

  tmpVar = NlToolbox::Math::cos(frequency);

  m_lp1_a1 = tmpVar * -2.f;
  m_lp1_b0 = (1.f - tmpVar) / 2.f;
  m_lp1_b1 = 1.f - tmpVar;

  tmpVar = NlToolbox::Math::sin(frequency) * 0.5f;

  m_lp1_a2 = 1.f - tmpVar;
  tmpVar = 1.f + tmpVar;

  m_lp1_a1 = m_lp1_a1 / tmpVar * -1.f;
  m_lp1_a2 = m_lp1_a2 / tmpVar * -1.f;
  m_lp1_b0 = m_lp1_b0 / tmpVar;
  m_lp1_b1 = m_lp1_b1 / tmpVar;

  //*************************** Biquad Lowpass 2 ***************************//
  frequency = signals.get<Signals::CAB_LPF>() * 1.333f;
  frequency = std::clamp(frequency, m_freqClip_min, m_freqClip_max);
  frequency *= m_warpConst_2PI;
  tmpVar = NlToolbox::Math::cos(frequency);

  m_lp2_a1 = tmpVar * -2.f;
  m_lp2_b0 = (1.f - tmpVar) / 2.f;
  m_lp2_b1 = 1.f - tmpVar;

  tmpVar = NlToolbox::Math::sin(frequency) * 0.5f;

  m_lp2_a2 = 1.f - tmpVar;
  tmpVar = 1.f + tmpVar;

  m_lp2_a1 = m_lp2_a1 / tmpVar * -1.f;
  m_lp2_a2 = m_lp2_a2 / tmpVar * -1.f;
  m_lp2_b0 = m_lp2_b0 / tmpVar;
  m_lp2_b1 = m_lp2_b1 / tmpVar;

  //**************************** Tilt Lowshelves ***************************//
  float tilt = std::pow(10.f, signals.get<Signals::CAB_TILT>() * 0.025f);
  //    float tilt = std::pow(1.059192f, signals.get(SignalLabel::CAB_TILT));       // alternative to pow(10.f, signals.get(SignalLabel::CAB_TILT) / 40.f)

  tmpVar = tilt + 1.f / tilt + 2.f;
  tmpVar = std::sqrt(tilt * tmpVar) * m_tiltOmegaSin;

  float coeff = (tilt + 1.f) + (m_tiltOmegaCos * (tilt - 1.f)) + tmpVar;
  m_ls1_a1 = ((tilt - 1.f) + (m_tiltOmegaCos * (tilt + 1.f))) * -2.f;
  m_ls1_a2 = (tilt + 1.f) + (m_tiltOmegaCos * (tilt - 1.f)) - tmpVar;
  m_ls1_b0 = ((tilt + 1.f) - (m_tiltOmegaCos * (tilt - 1.f)) + tmpVar) * tilt;
  m_ls1_b1 = ((tilt - 1.f) - (m_tiltOmegaCos * (tilt + 1.f))) * 2.f * tilt;
  m_ls1_b2 = ((tilt + 1.f) - (m_tiltOmegaCos * (tilt - 1.f)) - tmpVar) * tilt;

  m_ls1_a1 = m_ls1_a1 / coeff * -1.f;
  m_ls1_a2 = m_ls1_a2 / coeff * -1.f;
  m_ls1_b0 = m_ls1_b0 / coeff;
  m_ls1_b1 = m_ls1_b1 / coeff;
  m_ls1_b2 = m_ls1_b2 / coeff;

  tilt = std::pow(10.f, signals.get<Signals::CAB_TILT>() * -0.025f);
  //    tilt = std::pow(1.059192f, signals.get(SignalLabel::CAB_TILT) * -1.f);          // alternative to pow(10.f, signals.get(SignalLabel::CAB_TILT) / 40.f * -1.f)

  tmpVar = tilt + 1.f / tilt + 2.f;
  tmpVar = std::sqrt(tilt * tmpVar) * m_tiltOmegaSin;

  coeff = (tilt + 1.f) + (m_tiltOmegaCos * (tilt - 1.f)) + tmpVar;
  m_ls2_a1 = ((tilt - 1.f) + (m_tiltOmegaCos * (tilt + 1.f))) * -2.f;
  m_ls2_a2 = (tilt + 1.f) + (m_tiltOmegaCos * (tilt - 1.f)) - tmpVar;
  m_ls2_b0 = ((tilt + 1.f) - (m_tiltOmegaCos * (tilt - 1.f)) + tmpVar) * tilt;
  m_ls2_b1 = ((tilt - 1.f) - (m_tiltOmegaCos * (tilt + 1.f))) * 2.f * tilt;
  m_ls2_b2 = ((tilt + 1.f) - (m_tiltOmegaCos * (tilt - 1.f)) - tmpVar) * tilt;

  m_ls2_a1 = m_ls2_a1 / coeff * -1.f;
  m_ls2_a2 = m_ls2_a2 / coeff * -1.f;
  m_ls2_b0 = m_ls2_b0 / coeff;
  m_ls2_b1 = m_ls2_b1 / coeff;
  m_ls2_b2 = m_ls2_b2 / coeff;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_cabinet::apply(float _rawSample_L, float _rawSample_R, SignalStorage &signals)
{
  float tmpVar;

  //********************************* Drive ********************************//
  float sample_L = _rawSample_L * signals.get<Signals::CAB_DRV>();
  float sample_R = _rawSample_R * signals.get<Signals::CAB_DRV>();

  //************************** Biquad Highpass L ***************************//
  tmpVar = m_hp_b0 * sample_L;
  tmpVar += m_hp_b1 * m_hp_stateVar_L1;
  tmpVar += m_hp_b0 * m_hp_stateVar_L2;
  tmpVar += m_hp_a1 * m_hp_stateVar_L3;
  tmpVar += m_hp_a2 * m_hp_stateVar_L4;

  m_hp_stateVar_L2 = m_hp_stateVar_L1 + DNC_const;
  m_hp_stateVar_L1 = sample_L + DNC_const;
  m_hp_stateVar_L4 = m_hp_stateVar_L3 + DNC_const;
  m_hp_stateVar_L3 = tmpVar + DNC_const;

  sample_L = tmpVar;

  //************************** Biquad Highpass R ***************************//
  tmpVar = m_hp_b0 * sample_R;
  tmpVar += m_hp_b1 * m_hp_stateVar_R1;
  tmpVar += m_hp_b0 * m_hp_stateVar_R2;
  tmpVar += m_hp_a1 * m_hp_stateVar_R3;
  tmpVar += m_hp_a2 * m_hp_stateVar_R4;

  m_hp_stateVar_R2 = m_hp_stateVar_R1 + DNC_const;
  m_hp_stateVar_R1 = sample_R + DNC_const;
  m_hp_stateVar_R4 = m_hp_stateVar_R3 + DNC_const;
  m_hp_stateVar_R3 = tmpVar + DNC_const;

  sample_R = tmpVar;

  //*************************** Tilt Lowshelf L1 ***************************//
  tmpVar = m_ls1_b0 * sample_L;
  tmpVar += m_ls1_b1 * m_ls1_stateVar_L1;
  tmpVar += m_ls1_b2 * m_ls1_StateVar_L2;
  tmpVar += m_ls1_a1 * m_ls1_StateVar_L3;
  tmpVar += m_ls1_a2 * m_ls1_StateVar_L4;

  m_ls1_StateVar_L2 = m_ls1_stateVar_L1 + DNC_const;
  m_ls1_stateVar_L1 = sample_L + DNC_const;
  m_ls1_StateVar_L4 = m_ls1_StateVar_L3 + DNC_const;
  m_ls1_StateVar_L3 = tmpVar + DNC_const;

  sample_L = tmpVar;

  //*************************** Tilt Lowshelf R1 ***************************//
  tmpVar = m_ls1_b0 * sample_R;
  tmpVar += m_ls1_b1 * m_ls1_stateVar_R1;
  tmpVar += m_ls1_b2 * m_ls1_stateVar_R2;
  tmpVar += m_ls1_a1 * m_ls1_stateVar_R3;
  tmpVar += m_ls1_a2 * m_ls1_stateVar_R4;

  m_ls1_stateVar_R2 = m_ls1_stateVar_R1 + DNC_const;
  m_ls1_stateVar_R1 = sample_R + DNC_const;
  m_ls1_stateVar_R4 = m_ls1_stateVar_R3 + DNC_const;
  m_ls1_stateVar_R3 = tmpVar + DNC_const;

  sample_R = tmpVar;

  //******************************* Shaper L *******************************//
  sample_L *= signals.get<Signals::CAB_PRESAT>();
  tmpVar = sample_L;

  sample_L = NlToolbox::Math::sinP3_wrap(sample_L);
  sample_L = NlToolbox::Others::threeRanges(sample_L, tmpVar, signals.get<Signals::CAB_FLD>());

  tmpVar = sample_L * sample_L - m_hp30_stateVar_L;
  m_hp30_stateVar_L = tmpVar * m_hp30_b0 + m_hp30_stateVar_L + DNC_const;

  sample_L = NlToolbox::Others::parAsym(sample_L, tmpVar, signals.get<Signals::CAB_ASM>());
  sample_L *= signals.get<Signals::CAB_SAT>();

  //******************************* Shaper R *******************************//
  sample_R *= signals.get<Signals::CAB_PRESAT>();
  tmpVar = sample_R;

  sample_R = NlToolbox::Math::sinP3_wrap(sample_R);
  sample_R = NlToolbox::Others::threeRanges(sample_R, tmpVar, signals.get<Signals::CAB_FLD>());

  tmpVar = sample_R * sample_R - m_hp30_stateVar_R;
  m_hp30_stateVar_R = tmpVar * m_hp30_b0 + m_hp30_stateVar_R + DNC_const;

  sample_R = NlToolbox::Others::parAsym(sample_R, tmpVar, signals.get<Signals::CAB_ASM>());
  sample_R *= signals.get<Signals::CAB_SAT>();

  //*************************** Tilt Lowshelf L2 ***************************//
  tmpVar = m_ls2_b0 * sample_L;
  tmpVar += m_ls2_b1 * m_ls2_stateVar_L1;
  tmpVar += m_ls2_b2 * m_ls2_StateVar_L2;
  tmpVar += m_ls2_a1 * m_ls2_StateVar_L3;
  tmpVar += m_ls2_a2 * m_ls2_StateVar_L4;

  m_ls2_StateVar_L2 = m_ls2_stateVar_L1 + DNC_const;
  m_ls2_stateVar_L1 = sample_L + DNC_const;
  m_ls2_StateVar_L4 = m_ls2_StateVar_L3 + DNC_const;
  m_ls2_StateVar_L3 = tmpVar + DNC_const;

  sample_L = tmpVar;

  //*************************** Tilt Lowshelf R2 ***************************//
  tmpVar = m_ls2_b0 * sample_R;
  tmpVar += m_ls2_b1 * m_ls2_stateVar_R1;
  tmpVar += m_ls2_b2 * m_ls2_stateVar_R2;
  tmpVar += m_ls2_a1 * m_ls2_stateVar_R3;
  tmpVar += m_ls2_a2 * m_ls2_stateVar_R4;

  m_ls2_stateVar_R2 = m_ls2_stateVar_R1 + DNC_const;
  m_ls2_stateVar_R1 = sample_R + DNC_const;
  m_ls2_stateVar_R4 = m_ls2_stateVar_R3 + DNC_const;
  m_ls2_stateVar_R3 = tmpVar + DNC_const;

  sample_R = tmpVar;

  //************************* 2 x Biquad Lowpass L *************************//
  tmpVar = m_lp1_b0 * sample_L;
  tmpVar += m_lp1_b1 * m_lp1_stateVar_L1;
  tmpVar += m_lp1_b0 * m_lp1_stateVar_L2;
  tmpVar += m_lp1_a1 * m_lp1_stateVar_L3;
  tmpVar += m_lp1_a2 * m_lp1_stateVar_L4;

  m_lp1_stateVar_L2 = m_lp1_stateVar_L1 + DNC_const;
  m_lp1_stateVar_L1 = sample_L + DNC_const;
  m_lp1_stateVar_L4 = m_lp1_stateVar_L3 + DNC_const;
  m_lp1_stateVar_L3 = tmpVar + DNC_const;

  sample_L = tmpVar;

  tmpVar = m_lp2_b0 * sample_L;
  tmpVar += m_lp2_b1 * m_lp2_stateVar_L1;
  tmpVar += m_lp2_b0 * m_lp2_stateVar_L2;
  tmpVar += m_lp2_a1 * m_lp2_stateVar_L3;
  tmpVar += m_lp2_a2 * m_lp2_stateVar_L4;

  m_lp2_stateVar_L2 = m_lp2_stateVar_L1 + DNC_const;
  m_lp2_stateVar_L1 = sample_L + DNC_const;
  m_lp2_stateVar_L4 = m_lp2_stateVar_L3 + DNC_const;
  m_lp2_stateVar_L3 = tmpVar + DNC_const;

  sample_L = tmpVar;

  //************************* 2 x Biquad Lowpass R *************************//
  tmpVar = m_lp1_b0 * sample_R;
  tmpVar += m_lp1_b1 * m_lp1_stateVar_R1;
  tmpVar += m_lp1_b0 * m_lp1_stateVar_R2;
  tmpVar += m_lp1_a1 * m_lp1_stateVar_R3;
  tmpVar += m_lp1_a2 * m_lp1_stateVar_R4;

  m_lp1_stateVar_R2 = m_lp1_stateVar_R1 + DNC_const;
  m_lp1_stateVar_R1 = sample_R + DNC_const;
  m_lp1_stateVar_R4 = m_lp1_stateVar_R3 + DNC_const;
  m_lp1_stateVar_R3 = tmpVar + DNC_const;

  sample_R = tmpVar;

  tmpVar = m_lp2_b0 * sample_R;
  tmpVar += m_lp2_b1 * m_lp2_stateVar_R1;
  tmpVar += m_lp2_b0 * m_lp2_stateVar_R2;
  tmpVar += m_lp2_a1 * m_lp2_stateVar_R3;
  tmpVar += m_lp2_a2 * m_lp2_stateVar_R4;

  m_lp2_stateVar_R2 = m_lp2_stateVar_R1 + DNC_const;
  m_lp2_stateVar_R1 = sample_R + DNC_const;
  m_lp2_stateVar_R4 = m_lp2_stateVar_R3 + DNC_const;
  m_lp2_stateVar_R3 = tmpVar + DNC_const;

  sample_R = tmpVar;

  //******************************* Crossfade ******************************//
  m_out_L = NlToolbox::Crossfades::crossFade(_rawSample_L, sample_L, signals.get<Signals::CAB_DRY>(),
                                             signals.get<Signals::CAB_WET>());
  m_out_R = NlToolbox::Crossfades::crossFade(_rawSample_R, sample_R, signals.get<Signals::CAB_DRY>(),
                                             signals.get<Signals::CAB_WET>());
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_cabinet::resetDSP()
{
  m_out_L = 0.f;
  m_out_R = 0.f;

  //*************************** Biquad Highpass ****************************//

  m_hp_stateVar_L1 = 0.f;
  m_hp_stateVar_L2 = 0.f;
  m_hp_stateVar_L3 = 0.f;
  m_hp_stateVar_L4 = 0.f;
  m_hp_stateVar_R1 = 0.f;
  m_hp_stateVar_R2 = 0.f;
  m_hp_stateVar_R3 = 0.f;
  m_hp_stateVar_R4 = 0.f;

  //************************** Biquad Lowpasses ****************************//

  m_lp1_stateVar_L1 = 0.f;
  m_lp1_stateVar_L2 = 0.f;
  m_lp1_stateVar_L3 = 0.f;
  m_lp1_stateVar_L4 = 0.f;
  m_lp1_stateVar_R1 = 0.f;
  m_lp1_stateVar_R2 = 0.f;
  m_lp1_stateVar_R3 = 0.f;
  m_lp1_stateVar_R4 = 0.f;

  m_lp2_stateVar_L1 = 0.f;
  m_lp2_stateVar_L2 = 0.f;
  m_lp2_stateVar_L3 = 0.f;
  m_lp2_stateVar_L4 = 0.f;
  m_lp2_stateVar_R1 = 0.f;
  m_lp2_stateVar_R2 = 0.f;
  m_lp2_stateVar_R3 = 0.f;
  m_lp2_stateVar_R4 = 0.f;

  //*************************** Tilt Lowshelves ****************************//

  m_ls1_stateVar_L1 = 0.f;
  m_ls1_StateVar_L2 = 0.f;
  m_ls1_StateVar_L3 = 0.f;
  m_ls1_StateVar_L4 = 0.f;
  m_ls1_stateVar_R1 = 0.f;
  m_ls1_stateVar_R2 = 0.f;
  m_ls1_stateVar_R3 = 0.f;
  m_ls1_stateVar_R4 = 0.f;

  m_ls2_stateVar_L1 = 0.f;
  m_ls2_StateVar_L2 = 0.f;
  m_ls2_StateVar_L3 = 0.f;
  m_ls2_StateVar_L4 = 0.f;
  m_ls2_stateVar_R1 = 0.f;
  m_ls2_stateVar_R2 = 0.f;
  m_ls2_stateVar_R3 = 0.f;
  m_ls2_stateVar_R4 = 0.f;

  //***************************** 30 Hz Highpass ****************************//

  m_hp30_stateVar_L = 0.f;
  m_hp30_stateVar_R = 0.f;
}
