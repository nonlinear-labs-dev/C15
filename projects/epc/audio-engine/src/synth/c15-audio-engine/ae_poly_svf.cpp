#include "ae_poly_svf.h"

/******************************************************************************/
/** @file       ae_poly_svf.cpp
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      state variable filter
    @todo
*******************************************************************************/

Engine::PolyStateVariableFilter::PolyStateVariableFilter()
{
}

void Engine::PolyStateVariableFilter::init(const float _samplerate)
{
  m_out = 0.0f;
  m_warpConst_2PI = NlToolbox::Constants::twopi / _samplerate;
  m_first_int1_stateVar = m_first_int2_stateVar = m_second_int1_stateVar = m_second_int2_stateVar = m_first_sat_stateVar
      = m_second_sat_stateVar = 0.0f;
}

void Engine::PolyStateVariableFilter::apply(PolySignals &_signals, const PolyValue &_sampleA, const PolyValue &_sampleB,
                                            const PolyValue &_sampleComb)
{
  // damp, fmax (from res)
  auto damp = _signals.get(C15::Signals::Truepoly_Signals::SV_Flt_Res_Damp);
  auto fmax = _signals.get(C15::Signals::Truepoly_Signals::SV_Flt_Res_FMax);
  // sample mix
  auto tmpVar = _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_In_A_B);
  auto mixSample = (_sampleB * (1.0f - tmpVar)) + (_sampleA * tmpVar);
  tmpVar = _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_Comb_Mix);
  mixSample = (mixSample * (1.0f - std::abs(tmpVar))) + (_sampleComb * tmpVar);
  // fm
  tmpVar = _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_FM_A_B);
  auto fmVar = (_sampleA * (1.0f - tmpVar)) + (_sampleB * tmpVar);
  // 1st stage svf
  auto inputSample = mixSample + (m_first_sat_stateVar * 0.10f);
  auto omega = (_signals.get(C15::Signals::Truepoly_Signals::SV_Flt_F1_Cut)
                + (fmVar * _signals.get(C15::Signals::Truepoly_Signals::SV_Flt_F1_FM)))
      * m_warpConst_2PI;
  omega = std::clamp(omega, 0.0f, fmax);
  auto highpassOutput = inputSample - ((m_first_int1_stateVar * damp) + m_first_int2_stateVar);
  auto bandpassOutput = (highpassOutput * omega) + m_first_int1_stateVar;
  auto lowpassOutput = (bandpassOutput * omega) + m_first_int2_stateVar;
  m_first_int1_stateVar = bandpassOutput;
  m_first_int2_stateVar = lowpassOutput;
  tmpVar = _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_LBH_1);
  auto outputSample_1 = lowpassOutput * std::max(-tmpVar, 0.0f);
  outputSample_1 += bandpassOutput * (1.0f - std::abs(tmpVar));
  outputSample_1 += highpassOutput * std::max(tmpVar, 0.0f);
  // 1st stage parabol sat
  m_first_sat_stateVar = std::clamp(bandpassOutput, -2.0f, 2.0f);
  m_first_sat_stateVar *= 1.0f - (std::abs(m_first_sat_stateVar) * 0.250f);
  // 2nd stage svf
  inputSample = (outputSample_1 * _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_Par_3))
      + (mixSample * _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_Par_4)) + (m_second_sat_stateVar * 0.10f);
  omega = (_signals.get(C15::Signals::Truepoly_Signals::SV_Flt_F2_Cut)
           + (fmVar * _signals.get(C15::Signals::Truepoly_Signals::SV_Flt_F2_FM)))
      * m_warpConst_2PI;
  omega = std::clamp(omega, 0.0f, fmax);
  highpassOutput = inputSample - ((m_second_int1_stateVar * damp) + m_second_int2_stateVar);
  bandpassOutput = (highpassOutput * omega) + m_second_int1_stateVar;
  lowpassOutput = (bandpassOutput * omega) + m_second_int2_stateVar;
  m_second_int1_stateVar = bandpassOutput;
  m_second_int2_stateVar = lowpassOutput;
  tmpVar = _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_LBH_2);
  auto outputSample_2 = lowpassOutput * std::max(-tmpVar, 0.0f);
  outputSample_2 += bandpassOutput * (1.0f - std::abs(tmpVar));
  outputSample_2 += highpassOutput * std::max(tmpVar, 0.0f);
  // 2nd stage parabol sat
  m_second_sat_stateVar = std::clamp(bandpassOutput, -2.0f, 2.0f);
  m_second_sat_stateVar *= 1.0f - (std::abs(m_second_sat_stateVar) * 0.250f);
  // crossfades
  m_out = (outputSample_1 * _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_Par_1))
      + (outputSample_2 * _signals.get(C15::Signals::Quasipoly_Signals::SV_Flt_Par_2));
}

void Engine::PolyStateVariableFilter::resetDSP()
{
  m_out = m_first_int1_stateVar = m_first_int2_stateVar = m_second_int1_stateVar = m_second_int2_stateVar
      = m_first_sat_stateVar = m_second_sat_stateVar = 0.0f;
}
