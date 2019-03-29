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
#include "ParameterStorage.h"

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

  for(int32_t i = 0; i < dsp_number_of_voices; i++)
  {
    m_OscA_randVal_int[i] = i + 1;
    m_OscB_randVal_int[i] = i + 1 + 111;
  }

  m_OscA_mute = 1.0f;
  m_OscB_mute = 1.0f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/
void ae_soundgenerator::set(uint32_t voice, ParameterStorage &params)
{
  //*************************** Chirp Filter A *****************************//
  m_chiA_omega[voice] = params[OSC_A_CHI] * m_warpConst_PI;
  m_chiA_omega[voice] = NlToolbox::Math::tan(m_chiA_omega[voice]);

  m_chiA_a0[voice] = 1.f / (m_chiA_omega[voice] + 1.f);
  m_chiA_a1[voice] = m_chiA_omega[voice] - 1.f;

  //*************************** Chirp Filter B *****************************//
  m_chiB_omega[voice] = params[OSC_B_CHI] * m_warpConst_PI;
  m_chiB_omega[voice] = NlToolbox::Math::tan(m_chiB_omega[voice]);

  m_chiB_a0[voice] = 1.f / (m_chiB_omega[voice] + 1.f);
  m_chiB_a1[voice] = m_chiB_omega[voice] - 1.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::resetPhase(uint32_t voice, float _phase)
{
  m_oscA_phase[voice] = _phase;
  m_oscB_phase[voice] = _phase;
  /* */
  m_oscA_selfmix[voice] = 0.f;
  m_oscA_crossmix[voice] = 0.f;
  m_chiA_stateVar[voice] = 0.f;
  /* */
  m_oscB_selfmix[voice] = 0.f;
  m_oscB_crossmix[voice] = 0.f;
  m_chiB_stateVar[voice] = 0.f;
  /* */
  m_feedback_phase[voice] = 0.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::generate(FloatVector _feedbackSample, ParameterStorage &params)
{
  /*-{
    FloatVector a(0.f);
    a[0] = -10.f;
    a[1] = -1.f;
    a[2] = -0.5f;
    a[3] = -0.25f;
    a[4] = -0.125f;
    a[5] = 10.f;
    a[6] = 1.f;
    a[7] = 0.5f;
    a[8] = 0.25f;
    a[9] = 0.125f;

    auto resOld = sinP3_wrapOld(a);
    auto resNew = sinP3_wrap(a);

    std::cout << resOld[0] << resNew[0];
  }-*/

  //**************************** Modulation A ******************************//
  auto oscSampleA = m_oscA_selfmix * params.getParameterForAllVoices(OSC_A_PMSEA);
  oscSampleA = oscSampleA + m_oscB_crossmix * params.getParameterForAllVoices(OSC_A_PMBEB);
  oscSampleA = oscSampleA + m_feedback_phase * params.getParameterForAllVoices(OSC_A_PMFEC);

  //**************************** Oscillator A ******************************//
  oscSampleA -= (m_chiA_a1 * m_chiA_stateVar);  // Chirp IIR
  oscSampleA *= m_chiA_a0;

  auto tmpVarA = oscSampleA;

  oscSampleA = (oscSampleA + m_chiA_stateVar) * m_chiA_omega;  // Chirp FIR
  m_chiA_stateVar = tmpVarA + NlToolbox::Constants::DNC_const;

  oscSampleA += m_oscA_phase;

  oscSampleA
      += params.getParameterForAllVoices(OSC_A_PHS) + params.getParameterForAllVoices(UN_PHS);  // NEW Phase Offset
  oscSampleA += (-0.25f);                                                                       // Wrap
  oscSampleA = keepFractional(oscSampleA);

  for(size_t i = 0; i < dsp_number_of_voices; i++)
  {
    if(std::abs(m_oscA_phase_stateVar[i] - oscSampleA[i]) > 0.5f)  // Check edge
    {
      m_OscA_randVal_int[i] = m_OscA_randVal_int[i] * 1103515245 + 12345;
      m_OscA_randVal_float[i] = static_cast<float>(m_OscA_randVal_int[i]) * 4.5657e-10f;
    }
  }

  auto osc_freq = params.getParameterForAllVoices(OSC_A_FRQ);
  m_oscA_phaseInc = ((m_OscA_randVal_float * params.getParameterForAllVoices(OSC_A_FLUEC) * osc_freq) + osc_freq)
      * m_sample_interval;

  m_oscA_phase_stateVar = oscSampleA;

  m_oscA_phase += m_oscA_phaseInc;
  m_oscA_phase = keepFractional(m_oscA_phase);

  oscSampleA = m_OscA_mute * sinP3_noWrap(oscSampleA);

  //**************************** Modulation B ******************************//
  auto oscSampleB = m_oscB_selfmix * params.getParameterForAllVoices(OSC_B_PMSEB);
  oscSampleB = oscSampleB + m_oscA_crossmix * params.getParameterForAllVoices(OSC_B_PMAEA);
  oscSampleB = oscSampleB + m_feedback_phase * params.getParameterForAllVoices(OSC_B_PMFEC);

  //**************************** Oscillator B ******************************//
  oscSampleB -= (m_chiB_a1 * m_chiB_stateVar);  // Chirp IIR
  oscSampleB *= m_chiB_a0;

  auto tmpVarB = oscSampleB;

  oscSampleB = (oscSampleB + m_chiB_stateVar) * m_chiB_omega;  // Chirp FIR
  m_chiB_stateVar = tmpVarB + NlToolbox::Constants::DNC_const;

  oscSampleB += m_oscB_phase;

  oscSampleB
      += params.getParameterForAllVoices(OSC_B_PHS) + params.getParameterForAllVoices(UN_PHS);  // NEW Phase Offset
  oscSampleB += (-0.25f);                                                                       // Warp
  oscSampleB = keepFractional(oscSampleB);

  for(size_t i = 0; i < dsp_number_of_voices; i++)
  {
    if(std::abs(m_oscB_phase_stateVar[i] - oscSampleB[i]) > 0.5f)  // Check edge
    {
      m_OscB_randVal_int[i] = m_OscB_randVal_int[i] * 1103515245 + 12345;
      m_OscB_randVal_float[i] = static_cast<float>(m_OscB_randVal_int[i]) * 4.5657e-10f;
    }
  }
  osc_freq = params.getParameterForAllVoices(OSC_B_FRQ);
  m_oscB_phaseInc = ((m_OscB_randVal_float * params.getParameterForAllVoices(OSC_B_FLUEC) * osc_freq) + osc_freq)
      * m_sample_interval;

  m_oscB_phase_stateVar = oscSampleB;

  m_oscB_phase += m_oscB_phaseInc;
  m_oscB_phase = keepFractional(m_oscB_phase);

  oscSampleB = m_OscB_mute * sinP3_noWrap(oscSampleB);

  //******************************* Shaper A *******************************//
  auto shaperSampleA = oscSampleA * params.getParameterForAllVoices(SHP_A_DRVEA);
  auto tmpVarSA = shaperSampleA;

  shaperSampleA = sinP3_wrap(shaperSampleA);
  shaperSampleA = threeRanges(shaperSampleA, tmpVarSA, params.getParameterForAllVoices(SHP_A_FLD));

  auto tmpVarSSA = shaperSampleA * shaperSampleA + (-0.5f);

  shaperSampleA = parAsym(shaperSampleA, tmpVarSSA, params.getParameterForAllVoices(SHP_A_ASM));

  //******************************* Shaper B *******************************//
  auto shaperSampleB = oscSampleB * params.getParameterForAllVoices(SHP_B_DRVEB);
  auto tmpVarSB = shaperSampleB;

  shaperSampleB = sinP3_wrap(shaperSampleB);
  shaperSampleB = threeRanges(shaperSampleB, tmpVarSB, params.getParameterForAllVoices(SHP_B_FLD));

  auto tmpVarSSB = shaperSampleB * shaperSampleB + (-0.5f);

  shaperSampleB = parAsym(shaperSampleB, tmpVarSSB, params.getParameterForAllVoices(SHP_B_ASM));

  //****************************** Crossfades ******************************//
  m_oscA_selfmix = bipolarCrossFade(oscSampleA, shaperSampleA, params.getParameterForAllVoices(OSC_A_PMSSH));
  m_oscA_crossmix = bipolarCrossFade(oscSampleA, shaperSampleA, params.getParameterForAllVoices(OSC_B_PMASH));

  m_oscB_selfmix = bipolarCrossFade(oscSampleB, shaperSampleB, params.getParameterForAllVoices(OSC_B_PMSSH));
  m_oscB_crossmix = bipolarCrossFade(oscSampleB, shaperSampleB, params.getParameterForAllVoices(OSC_A_PMBSH));

  m_out_A = bipolarCrossFade(oscSampleA, shaperSampleA, params.getParameterForAllVoices(SHP_A_MIX));
  m_out_B = bipolarCrossFade(oscSampleB, shaperSampleB, params.getParameterForAllVoices(SHP_B_MIX));

  //******************* Envelope Influence (Magnitudes) ********************//
  m_out_A *= params.getParameterForAllVoices(ENV_A_MAG);
  m_out_B *= params.getParameterForAllVoices(ENV_B_MAG);

  //**************************** Feedback Mix ******************************//
  auto tmpVarOutA = _feedbackSample * params.getParameterForAllVoices(SHP_A_FBEC);
  m_out_A = unipolarCrossFade(m_out_A, tmpVarOutA, params.getParameterForAllVoices(SHP_A_FBM));

  auto tmpVarOutB = _feedbackSample * params.getParameterForAllVoices(SHP_B_FBEC);
  m_out_B = unipolarCrossFade(m_out_B, tmpVarOutB, params.getParameterForAllVoices(SHP_B_FBM));

  //************************** Ring Modulation *****************************//
  auto tmpVarMono = m_out_A * m_out_B;

  m_out_A = unipolarCrossFade(m_out_A, tmpVarMono, params.getParameterForAllVoices(SHP_A_RM));
  m_out_B = unipolarCrossFade(m_out_B, tmpVarMono, params.getParameterForAllVoices(SHP_B_RM));
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
