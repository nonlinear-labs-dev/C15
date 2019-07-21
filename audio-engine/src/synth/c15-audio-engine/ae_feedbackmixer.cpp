/******************************************************************************/
/** @file		ae_feedbackmixer.cpp
    @date		2017-07-30
    @version	1.0
    @author		Anton Schmied
    @brief		Feedbackmixer
*******************************************************************************/

#include "ae_feedbackmixer.h"
#include "SignalStorage.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_feedbackmixer::ae_feedbackmixer()
{
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_feedbackmixer::init(float _samplerate)
{
  m_out = 0.f;

  m_warpConst_PI = pi / _samplerate;
  m_freqClip_min = _samplerate / 24000.f;
  m_freqClip_max = _samplerate / 3.f;

  //***************************** 30Hz Highpass ****************************//
  m_hp30hz_b0 = (twopi / _samplerate) * 30.f;
  m_hp30hz_b0 = std::min(m_hp30hz_b0, 0.8f);

  m_hp30hz_stateVar = 0.f;

  //*************************** 1 pole Highpass ****************************//
  m_hp_b0 = 0.f;
  m_hp_b1 = 0.f;
  m_hp_a1 = 0.f;

  m_hp_stateVar_1 = 0.f;
  m_hp_stateVar_2 = 0.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_feedbackmixer::set(SignalStorage &signals, uint32_t _voiceID)
{
  auto omega = std::clamp(signals.get<Signals::FBM_HPF>()[_voiceID], m_freqClip_min, m_freqClip_max);
  omega = NlToolbox::Math::tan(omega * m_warpConst_PI);

  m_hp_a1[_voiceID] = (1.f - omega) / (1.f + omega);
  m_hp_b0[_voiceID] = 1.f / (1.f + omega);
  m_hp_b1[_voiceID] = (1.f / (1.f + omega)) * -1.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_feedbackmixer::apply(const FloatVector &_sampleComb, const FloatVector &_sampleSVF,
                             const FloatVector &_sampleFX, SignalStorage &signals)
{
  auto tmpVar = _sampleFX * signals.get<Signals::FBM_FX>() + _sampleComb * signals.get<Signals::FBM_CMB>()
      + _sampleSVF * signals.get<Signals::FBM_SVF>();

  m_out = m_hp_b0 * tmpVar;  // HP
  m_out += m_hp_b1 * m_hp_stateVar_1;
  m_out += m_hp_a1 * m_hp_stateVar_2;

  m_hp_stateVar_1 = tmpVar + DNC_const;
  m_hp_stateVar_2 = m_out + DNC_const;

  m_out *= signals.get<Signals::FBM_DRV>();

  tmpVar = m_out;
  m_out = sinP3_wrap(m_out);
  m_out = threeRanges(m_out, tmpVar, signals.get<Signals::FBM_FLD>());

  tmpVar = m_out * m_out;
  tmpVar -= m_hp30hz_stateVar;  // HP 30Hz
  m_hp30hz_stateVar = tmpVar * m_hp30hz_b0 + m_hp30hz_stateVar + NlToolbox::Constants::DNC_const;

  m_out = parAsym(m_out, tmpVar, signals.get<Signals::FBM_ASM>());
  m_out = m_out * signals.get<Signals::FBM_LVL>();
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_feedbackmixer::resetDSP()
{
  m_out = 0.f;

  //***************************** 30Hz Highpass ****************************//

  m_hp30hz_stateVar = 0.f;

  //*************************** 1 pole Highpass ****************************//

  m_hp_stateVar_1 = 0.f;
  m_hp_stateVar_2 = 0.f;
}
