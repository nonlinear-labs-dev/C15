/******************************************************************************/
/** @file           ae_soundgenerator.cpp
    @date           2018-03-22
    @version        1.0
    @author         Anton Schmied
    @brief          Soundgenerator which contains both Oscillators and Shapers
                    receiving the rendered parameter signals from the
                    param engine
    @todo
*******************************************************************************/

#include "ae_soundgenerator.h"
#include "SignalStorage.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_soundgenerator::ae_soundgenerator()
{
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::init(float _samplerate)
{
  m_out_A = 0.f;
  m_out_B = 0.f;

  m_sample_interval = 1.f / _samplerate;
  m_warpConst_PI = 3.14159f / _samplerate;

  m_feedback_phase = 0.f;

  m_chiA_stateVar = 0.f;
  m_chiB_stateVar = 0.f;

  m_chiA_omega = 0.f;
  m_chiA_a0 = 0.f;
  m_chiA_a1 = 0.f;
  m_chiB_omega = 0.f;
  m_chiB_a0 = 0.f;
  m_chiB_a1 = 0.f;

  for(int vn = 0; vn < dsp_number_of_voices; vn++)
  {
    m_OscA_randVal_int[vn] = vn + 1;
    m_OscB_randVal_int[vn] = vn + 1 + 111;
  }
}

/******************************************************************************/
/** @brief
*******************************************************************************/
void ae_soundgenerator::set(SignalStorage &signals, uint32_t voice)
{
  //*************************** Chirp Filter A *****************************//
  m_chiA_omega[voice] = signals.get<Signals::OSC_A_CHI>() * m_warpConst_PI;
  m_chiA_omega[voice] = NlToolbox::Math::tan(m_chiA_omega[voice]);

  m_chiA_a0[voice] = 1.f / (m_chiA_omega[voice] + 1.f);
  m_chiA_a1[voice] = m_chiA_omega[voice] - 1.f;

  //*************************** Chirp Filter B *****************************//
  m_chiB_omega[voice] = signals.get<Signals::OSC_B_CHI>() * m_warpConst_PI;
  m_chiB_omega[voice] = NlToolbox::Math::tan(m_chiB_omega[voice]);

  m_chiB_a0[voice] = 1.f / (m_chiB_omega[voice] + 1.f);
  m_chiB_a1[voice] = m_chiB_omega[voice] - 1.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::resetPhase(float _phase, uint32_t _voiceID)
{
  m_oscA_phase[_voiceID] = _phase;
  m_oscB_phase[_voiceID] = _phase;
  /* */
  m_oscA_selfmix[_voiceID] = 0.f;
  m_oscA_crossmix[_voiceID] = 0.f;
  m_chiA_stateVar[_voiceID] = 0.f;
  /* */
  m_oscB_selfmix[_voiceID] = 0.f;
  m_oscB_crossmix[_voiceID] = 0.f;
  m_chiB_stateVar[_voiceID] = 0.f;
  /* */
  m_feedback_phase[_voiceID] = 0.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::generate(const FloatVector &_feedbackSample, SignalStorage &signals)
{
  //**************************** Modulation A ******************************//
  auto oscSampleA = m_oscA_selfmix * signals.get<Signals::OSC_A_PMSEA>();
  oscSampleA = oscSampleA + m_oscB_crossmix * signals.get<Signals::OSC_A_PMBEB>();
  oscSampleA = oscSampleA + m_feedback_phase * signals.get<Signals::OSC_A_PMFEC>();

  //**************************** Oscillator A ******************************//
  oscSampleA -= (m_chiA_a1 * m_chiA_stateVar);  // Chirp IIR
  oscSampleA *= m_chiA_a0;

  auto tmpVarA = oscSampleA;

  oscSampleA = (oscSampleA + m_chiA_stateVar) * m_chiA_omega;  // Chirp FIR
  m_chiA_stateVar = tmpVarA + NlToolbox::Constants::DNC_const;

  oscSampleA += m_oscA_phase;

  oscSampleA += signals.get<Signals::OSC_A_PHS>() + signals.get<Signals::UN_PHS>();  // NEW Phase Offset
  oscSampleA += (-0.25f);                                                            // Wrap
  oscSampleA = keepFractional(oscSampleA);

  for(size_t i = 0; i < dsp_number_of_voices; i++)
  {
    if(std::abs(m_oscA_phase_stateVar[i] - oscSampleA[i]) > 0.5f)  // Check edge
    {
      m_OscA_randVal_int[i] = m_OscA_randVal_int[i] * 1103515245 + 12345;
      m_OscA_randVal_float[i] = static_cast<float>(m_OscA_randVal_int[i]) * 4.5657e-10f;
    }
  }

  auto osc_freq = signals.get<Signals::OSC_A_FRQ>();
  m_oscA_phaseInc
      = ((m_OscA_randVal_float * signals.get<Signals::OSC_A_FLUEC>() * osc_freq) + osc_freq) * m_sample_interval;

  m_oscA_phase_stateVar = oscSampleA;

  m_oscA_phase += m_oscA_phaseInc;
  m_oscA_phase = keepFractional(m_oscA_phase);

  oscSampleA = m_mute_state_A * sinP3_noWrap(oscSampleA);

  //**************************** Modulation B ******************************//
  auto oscSampleB = m_oscB_selfmix * signals.get<Signals::OSC_B_PMSEB>();
  oscSampleB = oscSampleB + m_oscA_crossmix * signals.get<Signals::OSC_B_PMAEA>();
  oscSampleB = oscSampleB + m_feedback_phase * signals.get<Signals::OSC_B_PMFEC>();

  //**************************** Oscillator B ******************************//
  oscSampleB -= (m_chiB_a1 * m_chiB_stateVar);  // Chirp IIR
  oscSampleB *= m_chiB_a0;

  auto tmpVarB = oscSampleB;

  oscSampleB = (oscSampleB + m_chiB_stateVar) * m_chiB_omega;  // Chirp FIR
  m_chiB_stateVar = tmpVarB + NlToolbox::Constants::DNC_const;

  oscSampleB += m_oscB_phase;

  oscSampleB += signals.get<Signals::OSC_B_PHS>() + signals.get<Signals::UN_PHS>();  // NEW Phase Offset
  oscSampleB += (-0.25f);                                                            // Warp
  oscSampleB = keepFractional(oscSampleB);

  for(size_t i = 0; i < dsp_number_of_voices; i++)
  {
    if(std::abs(m_oscB_phase_stateVar[i] - oscSampleB[i]) > 0.5f)  // Check edge
    {
      m_OscB_randVal_int[i] = m_OscB_randVal_int[i] * 1103515245 + 12345;
      m_OscB_randVal_float[i] = static_cast<float>(m_OscB_randVal_int[i]) * 4.5657e-10f;
    }
  }

  osc_freq = signals.get<Signals::OSC_B_FRQ>();
  m_oscB_phaseInc
      = ((m_OscB_randVal_float * signals.get<Signals::OSC_B_FLUEC>() * osc_freq) + osc_freq) * m_sample_interval;

  m_oscB_phase_stateVar = oscSampleB;

  m_oscB_phase += m_oscB_phaseInc;
  m_oscB_phase = keepFractional(m_oscB_phase);

  oscSampleB = m_mute_state_B * sinP3_noWrap(oscSampleB);

  //******************************* Shaper A *******************************//
  auto shaperSampleA = oscSampleA * signals.get<Signals::SHP_A_DRVEA>();
  auto tmpVarShapeA = shaperSampleA;

  shaperSampleA = sinP3_wrap(shaperSampleA);
  shaperSampleA = threeRanges(shaperSampleA, tmpVarShapeA, signals.get<Signals::SHP_A_FLD>());

  auto tmpVarShapeAQ = shaperSampleA * shaperSampleA + (-0.5f);

  shaperSampleA = parAsym(shaperSampleA, tmpVarShapeAQ, signals.get<Signals::SHP_A_ASM>());

  //******************************* Shaper B *******************************//
  auto shaperSampleB = oscSampleB * signals.get<Signals::SHP_B_DRVEB>();
  auto tmpVarShapeB = shaperSampleB;

  shaperSampleB = sinP3_wrap(shaperSampleB);
  shaperSampleB = threeRanges(shaperSampleB, tmpVarShapeB, signals.get<Signals::SHP_B_FLD>());

  auto tmpVarShapeBQ = shaperSampleB * shaperSampleB + (-0.5f);

  shaperSampleB = parAsym(shaperSampleB, tmpVarShapeBQ, signals.get<Signals::SHP_B_ASM>());

  //****************************** Crossfades ******************************//
  m_oscA_selfmix = bipolarCrossFade(oscSampleA, shaperSampleA, signals.get<Signals::OSC_A_PMSSH>());
  m_oscA_crossmix = bipolarCrossFade(oscSampleA, shaperSampleA, signals.get<Signals::OSC_B_PMASH>());

  m_oscB_selfmix = bipolarCrossFade(oscSampleB, shaperSampleB, signals.get<Signals::OSC_B_PMSSH>());
  m_oscB_crossmix = bipolarCrossFade(oscSampleB, shaperSampleB, signals.get<Signals::OSC_A_PMBSH>());

  m_out_A = bipolarCrossFade(oscSampleA, shaperSampleA, signals.get<Signals::SHP_A_MIX>());
  m_out_B = bipolarCrossFade(oscSampleB, shaperSampleB, signals.get<Signals::SHP_B_MIX>());

  //******************* Envelope Influence (Magnitudes) ********************//
  m_out_A *= signals.get<Signals::ENV_A_MAG>();
  m_out_B *= signals.get<Signals::ENV_B_MAG>();

  //**************************** Feedback Mix ******************************//
  auto tmpVarFBA = _feedbackSample * signals.get<Signals::SHP_A_FBEC>();
  m_out_A = unipolarCrossFade(m_out_A, tmpVarFBA, signals.get<Signals::SHP_A_FBM>());

  auto tmpVarFBB = _feedbackSample * signals.get<Signals::SHP_B_FBEC>();
  m_out_B = unipolarCrossFade(m_out_B, tmpVarFBB, signals.get<Signals::SHP_B_FBM>());

  //************************** Ring Modulation *****************************//
  auto res = m_out_A * m_out_B;

  m_out_A = unipolarCrossFade(m_out_A, res, signals.get<Signals::SHP_A_RM>());
  m_out_B = unipolarCrossFade(m_out_B, res, signals.get<Signals::SHP_B_RM>());
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::resetDSP()
{
  m_out_A = 0.f;
  m_out_B = 0.f;

  m_chiA_stateVar = 0.f;
  m_chiB_stateVar = 0.f;

  m_oscA_selfmix = 0.f;
  m_oscA_crossmix = 0.f;
  m_oscA_phase_stateVar = 0.f;

  m_oscB_selfmix = 0.f;
  m_oscB_crossmix = 0.f;
  m_oscB_phase_stateVar = 0.f;

  m_feedback_phase = 0.f;
}
