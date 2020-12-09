#include "ae_poly_out_mix.h"

/******************************************************************************/
/** @file       ae_poly_out_mix.cpp
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      output mixer
    @todo
*******************************************************************************/

Engine::PolyOutputMixer::PolyOutputMixer()
{
}

void Engine::PolyOutputMixer::init(const float _samplerate, const uint32_t _numOfVoices)
{
  m_out_l = m_out_r = 0.0f;
  const float norm_omega = NlToolbox::Constants::twopi / _samplerate;
  m_hp30hz_b0 = norm_omega * 30.0f;  // 30 Hz - no clipping needed
  m_hp30hz_stateVar_L = m_hp30hz_stateVar_R = 0.0f;
  m_hp_b0 = norm_omega * 12.978f;  // 8 ST (12.978 Hz) - no clipping needed
  m_hp_stateVar_L = m_hp_stateVar_R = 0.0f;
}

void Engine::PolyOutputMixer::combine(PolySignals &_signals, const PolyValue &_voiceLevel, const PolyValue &_sampleA,
                                      const PolyValue &_sampleB, const PolyValue &_sampleComb,
                                      const PolyValue &_sampleSVFilter)
{
  // left
  auto mainSample = (_signals.get(C15::Signals::Truepoly_Signals::Out_Mix_A_Left) * _sampleA)
      + (_signals.get(C15::Signals::Truepoly_Signals::Out_Mix_B_Left) * _sampleB)
      + (_signals.get(C15::Signals::Truepoly_Signals::Out_Mix_Comb_Left) * _sampleComb)
      + (_signals.get(C15::Signals::Truepoly_Signals::Out_Mix_SVF_Left) * _sampleSVFilter);
  mainSample *= _signals.get(C15::Signals::Quasipoly_Signals::Out_Mix_Drive);
  auto tmpSample = mainSample;
  mainSample = sinP3_wrap(tmpSample);
  mainSample = threeRanges(mainSample, tmpSample, _signals.get(C15::Signals::Quasipoly_Signals::Out_Mix_Fold));
  auto tmpSampleQuad = mainSample * mainSample;
  tmpSampleQuad = tmpSampleQuad - m_hp30hz_stateVar_L;
  m_hp30hz_stateVar_L = tmpSampleQuad * m_hp30hz_b0 + m_hp30hz_stateVar_L;
  mainSample = parAsym(mainSample, tmpSampleQuad, _signals.get(C15::Signals::Quasipoly_Signals::Out_Mix_Asym));
  m_out_l = sumUp(mainSample * _voiceLevel);
  // right
  mainSample = (_signals.get(C15::Signals::Truepoly_Signals::Out_Mix_A_Right) * _sampleA)
      + (_signals.get(C15::Signals::Truepoly_Signals::Out_Mix_B_Right) * _sampleB)
      + (_signals.get(C15::Signals::Truepoly_Signals::Out_Mix_Comb_Right) * _sampleComb)
      + (_signals.get(C15::Signals::Truepoly_Signals::Out_Mix_SVF_Right) * _sampleSVFilter);
  mainSample *= _signals.get(C15::Signals::Quasipoly_Signals::Out_Mix_Drive);
  auto tmpVar = mainSample;
  mainSample = sinP3_wrap(mainSample);
  mainSample = threeRanges(mainSample, tmpVar, _signals.get(C15::Signals::Quasipoly_Signals::Out_Mix_Fold));
  tmpVar = mainSample * mainSample;
  tmpVar = tmpVar - m_hp30hz_stateVar_R;
  m_hp30hz_stateVar_R = tmpVar * m_hp30hz_b0 + m_hp30hz_stateVar_R;
  mainSample = parAsym(mainSample, tmpVar, _signals.get(C15::Signals::Quasipoly_Signals::Out_Mix_Asym));
  m_out_r = sumUp(mainSample * _voiceLevel);
}

void Engine::PolyOutputMixer::filter_level(PolySignals &_signals)
{
  // HP L
  float tmpVar = m_out_l - m_hp_stateVar_L;
  m_hp_stateVar_L = tmpVar * m_hp_b0 + m_hp_stateVar_L;
  // Out L
  m_out_l = tmpVar * _signals.get(C15::Signals::Quasipoly_Signals::Out_Mix_Lvl);
  // HP R
  tmpVar = m_out_r - m_hp_stateVar_R;
  m_hp_stateVar_R = tmpVar * m_hp_b0 + m_hp_stateVar_R;
  // Out R
  m_out_r = tmpVar * _signals.get(C15::Signals::Quasipoly_Signals::Out_Mix_Lvl);
}

void Engine::PolyOutputMixer::resetDSP()
{
  m_out_l = m_out_r = 0.0f;
  m_hp30hz_stateVar_L = m_hp30hz_stateVar_R = 0.0f;
  m_hp_stateVar_L = m_hp_stateVar_R = 0.0f;
}
