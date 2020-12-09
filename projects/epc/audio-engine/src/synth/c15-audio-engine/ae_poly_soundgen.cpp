#include "ae_poly_soundgen.h"

/******************************************************************************/
/** @file       ae_poly_soundgen.cpp
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      new soundgenerator
    @todo
*******************************************************************************/

Engine::PolySoundGenerator::PolySoundGenerator()
{
}

void Engine::PolySoundGenerator::init(const uint32_t _voices, const float _samplerate)
{
  m_feedback_phase_A = m_feedback_phase_B = m_out_A = m_out_B = 0.0f;
  m_sample_interval = 1.0f / _samplerate;
  m_warpConst_PI = 3.14159f / _samplerate;
  m_chiA_stateVar = m_chiB_stateVar = 0.0f;
  m_chiA_omega = m_chiA_a0 = m_chiA_a1 = 0.0f;
  m_chiB_omega = m_chiB_a0 = m_chiB_a1 = 0.0f;
  m_voices = _voices;
  for(int vn = 0; vn < _voices; vn++)
  {
    m_OscA_randVal_int[vn] = vn + 1;
    m_OscB_randVal_int[vn] = vn + 1 + 111;
  }
}
void Engine::PolySoundGenerator::set(PolySignals &_signals, const uint32_t _voiceId)
{
  m_chiA_omega[_voiceId] = _signals.get(C15::Signals::Quasipoly_Signals::Osc_A_Chirp) * m_warpConst_PI;
  m_chiA_omega[_voiceId] = NlToolbox::Math::tan(m_chiA_omega[_voiceId]);
  m_chiA_a0[_voiceId] = 1.0f / (m_chiA_omega[_voiceId] + 1.0f);
  m_chiA_a1[_voiceId] = m_chiA_omega[_voiceId] - 1.0f;
  m_chiB_omega[_voiceId] = _signals.get(C15::Signals::Quasipoly_Signals::Osc_B_Chirp) * m_warpConst_PI;
  m_chiB_omega[_voiceId] = NlToolbox::Math::tan(m_chiB_omega[_voiceId]);
  m_chiB_a0[_voiceId] = 1.0f / (m_chiB_omega[_voiceId] + 1.0f);
  m_chiB_a1[_voiceId] = m_chiB_omega[_voiceId] - 1.0f;
}

void Engine::PolySoundGenerator::generate(PolySignals &_signals, const PolyValue _feedbackSignal)
{
  // modulation a
  auto oscSampleA = m_oscA_selfmix * _signals.get(C15::Signals::Truepoly_Signals::Osc_A_PM_Self_Env_A);
  oscSampleA = oscSampleA + m_oscB_crossmix * _signals.get(C15::Signals::Truepoly_Signals::Osc_A_PM_B_Env_B);
  oscSampleA = oscSampleA + m_feedback_phase_A * _signals.get(C15::Signals::Truepoly_Signals::Osc_A_PM_FB_Env_C);
  // oscillator a
  oscSampleA -= (m_chiA_a1 * m_chiA_stateVar);  // chirp IIR
  oscSampleA *= m_chiA_a0;
  auto tmpVarA = oscSampleA;
  oscSampleA = (oscSampleA + m_chiA_stateVar) * m_chiA_omega;  // chirp FIR
  m_chiA_stateVar = tmpVarA;
  oscSampleA += m_oscA_phase;
  oscSampleA += _signals.get(C15::Signals::Quasipoly_Signals::Osc_A_Phase)
      + _signals.get(C15::Signals::Truepoly_Signals::Unison_PolyPhase);
  oscSampleA += (-0.25f);  // wrap
  oscSampleA = keepFractional(oscSampleA);
  // edge detection, fluctuation a
  const PolyInt edgeA((std::abs(m_oscA_phase_stateVar - oscSampleA) > 0.5f));  // contains 0 or -1 (instead of +1) ...
  m_OscA_randVal_int -= edgeA * ((m_OscA_randVal_int * 1103515244) + 12345);   // ... so: subtract instead of add
  m_OscA_randVal_float = static_cast<PolyValue>(m_OscA_randVal_int) * 4.5657e-10f;
  auto osc_freq = _signals.get(C15::Signals::Truepoly_Signals::Osc_A_Freq);
  m_oscA_phaseInc
      = ((m_OscA_randVal_float * _signals.get(C15::Signals::Truepoly_Signals::Osc_A_Fluct_Env_C) * osc_freq) + osc_freq)
      * m_sample_interval;
  m_oscA_phase_stateVar = oscSampleA;
  m_oscA_phase += m_oscA_phaseInc;
  m_oscA_phase = keepFractional(m_oscA_phase);
  oscSampleA = m_mute_state_A * sinP3_noWrap(oscSampleA);
  // modulation b
  auto oscSampleB = m_oscB_selfmix * _signals.get(C15::Signals::Truepoly_Signals::Osc_B_PM_Self_Env_B);
  oscSampleB = oscSampleB + m_oscA_crossmix * _signals.get(C15::Signals::Truepoly_Signals::Osc_B_PM_A_Env_A);
  oscSampleB = oscSampleB + m_feedback_phase_B * _signals.get(C15::Signals::Truepoly_Signals::Osc_B_PM_FB_Env_C);
  // oscillator b
  oscSampleB -= (m_chiB_a1 * m_chiB_stateVar);  // cChirp IIR
  oscSampleB *= m_chiB_a0;
  auto tmpVarB = oscSampleB;
  oscSampleB = (oscSampleB + m_chiB_stateVar) * m_chiB_omega;  // chirp FIR
  m_chiB_stateVar = tmpVarB;
  oscSampleB += m_oscB_phase;
  oscSampleB += _signals.get(C15::Signals::Quasipoly_Signals::Osc_B_Phase)
      + _signals.get(C15::Signals::Truepoly_Signals::Unison_PolyPhase);
  oscSampleB += (-0.25f);  // wrap
  oscSampleB = keepFractional(oscSampleB);
  // edge detection, fluctuation b
  const PolyInt edgeB((std::abs(m_oscB_phase_stateVar - oscSampleB) > 0.5f));  // contains 0 or -1 (instead of +1) ...
  m_OscB_randVal_int -= edgeB * ((m_OscB_randVal_int * 1103515244) + 12345);   // ... so: subtract instead of add
  m_OscB_randVal_float = static_cast<PolyValue>(m_OscB_randVal_int) * 4.5657e-10f;
  osc_freq = _signals.get(C15::Signals::Truepoly_Signals::Osc_B_Freq);
  m_oscB_phaseInc
      = ((m_OscB_randVal_float * _signals.get(C15::Signals::Truepoly_Signals::Osc_B_Fluct_Env_C) * osc_freq) + osc_freq)
      * m_sample_interval;
  m_oscB_phase_stateVar = oscSampleB;
  m_oscB_phase += m_oscB_phaseInc;
  m_oscB_phase = keepFractional(m_oscB_phase);
  oscSampleB = m_mute_state_B * sinP3_noWrap(oscSampleB);
  // shaper a
  auto shaperSampleA = oscSampleA * _signals.get(C15::Signals::Truepoly_Signals::Shp_A_Drive_Env_A);
  auto tmpVarShapeA = shaperSampleA;
  shaperSampleA = sinP3_wrap(shaperSampleA);
  shaperSampleA = threeRanges(shaperSampleA, tmpVarShapeA, _signals.get(C15::Signals::Quasipoly_Signals::Shp_A_Fold));
  auto tmpVarShapeAQ = shaperSampleA * shaperSampleA + (-0.5f);
  shaperSampleA = parAsym(shaperSampleA, tmpVarShapeAQ, _signals.get(C15::Signals::Quasipoly_Signals::Shp_A_Asym));
  // shaper b
  auto shaperSampleB = oscSampleB * _signals.get(C15::Signals::Truepoly_Signals::Shp_B_Drive_Env_B);
  auto tmpVarShapeB = shaperSampleB;
  shaperSampleB = sinP3_wrap(shaperSampleB);
  shaperSampleB = threeRanges(shaperSampleB, tmpVarShapeB, _signals.get(C15::Signals::Quasipoly_Signals::Shp_B_Fold));
  auto tmpVarShapeBQ = shaperSampleB * shaperSampleB + (-0.5f);
  shaperSampleB = parAsym(shaperSampleB, tmpVarShapeBQ, _signals.get(C15::Signals::Quasipoly_Signals::Shp_B_Asym));
  // crossfades
  m_oscA_selfmix
      = bipolarCrossFade(oscSampleA, shaperSampleA, _signals.get(C15::Signals::Quasipoly_Signals::Osc_A_PM_Self_Shp));
  m_oscA_crossmix
      = bipolarCrossFade(oscSampleA, shaperSampleA, _signals.get(C15::Signals::Quasipoly_Signals::Osc_B_PM_A_Shp));

  m_oscB_selfmix
      = bipolarCrossFade(oscSampleB, shaperSampleB, _signals.get(C15::Signals::Quasipoly_Signals::Osc_B_PM_Self_Shp));
  m_oscB_crossmix
      = bipolarCrossFade(oscSampleB, shaperSampleB, _signals.get(C15::Signals::Quasipoly_Signals::Osc_A_PM_B_Shp));
  m_out_A = bipolarCrossFade(oscSampleA, shaperSampleA, _signals.get(C15::Signals::Quasipoly_Signals::Shp_A_Mix));
  m_out_B = bipolarCrossFade(oscSampleB, shaperSampleB, _signals.get(C15::Signals::Quasipoly_Signals::Shp_B_Mix));
  // env influence (magnitudes)
  m_out_A *= _signals.get(C15::Signals::Truepoly_Signals::Env_A_Mag);
  m_out_B *= _signals.get(C15::Signals::Truepoly_Signals::Env_B_Mag);
  // fb mix
  auto tmpVarFBA = _feedbackSignal * _signals.get(C15::Signals::Truepoly_Signals::Shp_A_FB_Env_C);
  m_out_A = unipolarCrossFade(m_out_A, tmpVarFBA, _signals.get(C15::Signals::Quasipoly_Signals::Shp_A_FB_Mix));
  auto tmpVarFBB = _feedbackSignal * _signals.get(C15::Signals::Truepoly_Signals::Shp_B_FB_Env_C);
  m_out_B = unipolarCrossFade(m_out_B, tmpVarFBB, _signals.get(C15::Signals::Quasipoly_Signals::Shp_B_FB_Mix));
  // ring mod
  auto res = m_out_A * m_out_B;
  m_out_A = unipolarCrossFade(m_out_A, res, _signals.get(C15::Signals::Quasipoly_Signals::Shp_A_Ring_Mod));
  m_out_B = unipolarCrossFade(m_out_B, res, _signals.get(C15::Signals::Quasipoly_Signals::Shp_B_Ring_Mod));
}

void Engine::PolySoundGenerator::resetPhase(const uint32_t _voiceId, const bool _rstA, const bool _rstB)
{
  if(_rstA)
  {
    m_feedback_phase_A[_voiceId] = m_oscA_phase[_voiceId] = m_oscA_selfmix[_voiceId] = m_oscA_crossmix[_voiceId]
        = m_chiA_stateVar[_voiceId] = 0.0f;
  }
  if(_rstB)
  {
    m_feedback_phase_B[_voiceId] = m_oscB_phase[_voiceId] = m_oscB_selfmix[_voiceId] = m_oscB_crossmix[_voiceId]
        = m_chiB_stateVar[_voiceId] = 0.0f;
  }
}

void Engine::PolySoundGenerator::resetDSP()
{
  m_feedback_phase_A = m_feedback_phase_B = m_out_A = m_out_B = 0.0f;
  m_chiA_stateVar = m_chiB_stateVar = 0.0f;
  m_oscA_selfmix = m_oscA_crossmix = m_oscA_phase_stateVar = 0.0f;
  m_oscB_selfmix = m_oscB_crossmix = m_oscB_phase_stateVar = 0.0f;
}
