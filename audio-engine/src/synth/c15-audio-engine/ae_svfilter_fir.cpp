/******************************************************************************/
/** @file           ae_combfilter.cpp
    @date           2018-05-10
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter with FIR Filter Version and FM+F clipping
    @todo
*******************************************************************************/

#include "ae_svfilter_fir.h"
#include "ParameterStorage.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_svfilter_fir::ae_svfilter_fir()
{
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_svfilter_fir::init(float _samplerate)
{
  m_out = 0.f;

  m_warpConst_2PI = 6.28319f / _samplerate;

  m_first_fir_stateVar = 0.f;
  m_second_fir_stateVar = 0.f;
  m_first_int1_stateVar = 0.f;
  m_first_int2_stateVar = 0.f;
  m_second_int1_stateVar = 0.f;
  m_second_int2_stateVar = 0.f;

  m_first_sv_sample = 0.f;
  m_first_sat_stateVar = 0.f;
  m_second_sat_stateVar = 0.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_svfilter_fir::apply(float _sampleA, float _sampleB, float _sampleComb, ParameterStorage &params)
{
  float tmpRes = params[SVF_RES];

  //******************************** Sample Mix ****************************//
  float tmpVar = params[SVF_AB];
  float mixSample = _sampleB * (1.f - tmpVar) + _sampleA * tmpVar;
  tmpVar = params[SVF_CMIX];
  mixSample = mixSample * (1.f - std::abs(tmpVar)) + _sampleComb * tmpVar;

  //************************** Frequency Modulation ************************//
  float fmVar = _sampleA * params[SVF_FMAB] + _sampleB * (1.f - params[SVF_FMAB]);

  //************************** 1st Stage SV FILTER *************************//
  float inputSample = mixSample + (m_first_sat_stateVar * 0.1f);

  float omega = (params[SVF_F1_CUT] + fmVar * params[SVF_F1_FM]) * m_warpConst_2PI;
  omega = std::clamp(omega, 0.f, test_svf_fm_limit);  /// initially 1.5f

  float attenuation = ((2.f + omega) * (2.f - omega) * tmpRes) / (((tmpRes * omega) + (2.f - omega)) * 2.f);

  float firOut = (m_first_fir_stateVar + inputSample) * 0.25f;
  m_first_fir_stateVar = inputSample + DNC_const;

  tmpVar = firOut - (attenuation * m_first_int1_stateVar + m_first_int2_stateVar);

  float int1Out = tmpVar * omega + m_first_int1_stateVar;
  float int2Out = int1Out * omega + m_first_int2_stateVar;

  float lowpassOutput = int2Out + m_first_int2_stateVar;
  float bandpassOutput = int1Out + int1Out;
  float highpassOutput = inputSample - (int1Out * attenuation + lowpassOutput);

  m_first_int1_stateVar = int1Out + DNC_const;
  m_first_int2_stateVar = int2Out + DNC_const;

  float outputSample_1 = lowpassOutput * std::max(-(params[SVF_LBH_1]), 0.f);
  outputSample_1 += (bandpassOutput * (1.f - std::abs(params[SVF_LBH_1])));
  outputSample_1 += (highpassOutput * std::max((float) params[SVF_LBH_1], 0.f));

  //************************** 1st Stage Parabol Sat ***********************//
  m_first_sat_stateVar = std::clamp(bandpassOutput, -2.f, 2.f);
  m_first_sat_stateVar *= (1.f - std::abs(m_first_sat_stateVar) * 0.25f);

  //************************** 2nd Stage SV FILTER *************************//
  inputSample = (outputSample_1 * params[SVF_PAR_3]) + (mixSample * params[SVF_PAR_4]) + (m_second_sat_stateVar * 0.1f);

  omega = (params[SVF_F2_CUT] + fmVar * params[SVF_F2_FM]) * m_warpConst_2PI;
  omega = std::clamp(omega, 0.f, test_svf_fm_limit);  /// initially 1.5f

  attenuation = ((2.f + omega) * (2.f - omega) * tmpRes) / (((tmpRes * omega) + (2.f - omega)) * 2.f);

  firOut = (m_second_fir_stateVar + inputSample) * 0.25f;
  m_second_fir_stateVar = inputSample + DNC_const;

  tmpVar = firOut - (attenuation * m_second_int1_stateVar + m_second_int2_stateVar);

  int1Out = tmpVar * omega + m_second_int1_stateVar;
  int2Out = int1Out * omega + m_second_int2_stateVar;

  lowpassOutput = int2Out + m_second_int2_stateVar;
  bandpassOutput = int1Out + int1Out;
  highpassOutput = inputSample - (int1Out * attenuation + lowpassOutput);

  m_second_int1_stateVar = int1Out + DNC_const;
  m_second_int2_stateVar = int2Out + DNC_const;

  tmpVar = lowpassOutput * std::max(-(params[SVF_LBH_2]), 0.f);
  tmpVar += (bandpassOutput * (1.f - std::abs(params[SVF_LBH_2])));
  tmpVar += (highpassOutput * std::max((float) params[SVF_LBH_2], 0.f));

  //************************* 2nd Stage Parabol Sat ************************//
  m_second_sat_stateVar = std::clamp(bandpassOutput, -2.f, 2.f);
  m_second_sat_stateVar *= (1.f - std::abs(m_second_sat_stateVar) * 0.25f);

  //****************************** Crossfades ******************************//
  m_out = (outputSample_1 * params[SVF_PAR_1]) + (tmpVar * params[SVF_PAR_2]);
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_svfilter_fir::resetDSP()
{
  m_out = 0.f;

  m_first_fir_stateVar = 0.f;
  m_second_fir_stateVar = 0.f;
  m_first_int1_stateVar = 0.f;
  m_first_int2_stateVar = 0.f;
  m_second_int1_stateVar = 0.f;
  m_second_int2_stateVar = 0.f;

  m_first_sv_sample = 0.f;
  m_first_sat_stateVar = 0.f;
  m_second_sat_stateVar = 0.f;
}
