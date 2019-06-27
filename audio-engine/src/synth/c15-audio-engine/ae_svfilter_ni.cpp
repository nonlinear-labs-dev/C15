/******************************************************************************/
/** @file           ae_svfilter.cpp
    @date           2019-06-26
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter as used in the primary module
    @todo
*******************************************************************************/

#include "ae_svfilter_ni.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_svfilter_ni::ae_svfilter_ni()
{

}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_svfilter_ni::init(float _samplerate)
{
    m_out = 0.f;

    m_warpConst_2PI = 6.28319f / _samplerate;

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

void ae_svfilter_ni::apply(float _sampleA, float _sampleB, float _sampleComb, float *_signal)
{
    float tmpRes = _signal[SVF_RES];

    /* Setting max clipping for omega later on - this should go into the paramengine*/
    float omegaMax =  0.7352f + 0.2930f * tmpRes * (1.3075f + tmpRes);

    //******************************** Sample Mix ****************************//
    float tmpVar = _signal[SVF_AB];
    float mixSample = _sampleB * (1.f - tmpVar) + _sampleA * tmpVar;
    tmpVar = _signal[SVF_CMIX];
    mixSample = mixSample * (1.f - std::abs(tmpVar)) + _sampleComb * tmpVar;


    //************************** Frequency Modulation ************************//
    float fmVar = _sampleA * _signal[SVF_FMAB] + _sampleB * (1.f - _signal[SVF_FMAB]);


    //************************** 1st Stage SV FILTER *************************//
    float inputSample = mixSample + (m_first_sat_stateVar * 0.1f);

    float omega = (_signal[SVF_F1_CUT] + fmVar * _signal[SVF_F1_FM]) * m_warpConst_2PI;

    /*Here we will now perofmr clipping depending on the resonance!*/
    omega = std::clamp(omega, 0.f, omegaMax);

    /*NI suggests a different approach ... */
//    float attenuation = ((2.f + omega) * (2.f - omega) * tmpRes)
//                      / (((tmpRes * omega) + (2.f - omega)) * 2.f);
    float attenuation = 2.f - 2.f * tmpRes;

    float highpassOutput = inputSample - (m_first_int1_stateVar * attenuation + m_first_int2_stateVar);
    float bandpassOutput = highpassOutput * omega + m_first_int1_stateVar;
    float lowpassOutput  = bandpassOutput * omega + m_first_int2_stateVar;

    m_first_int1_stateVar = bandpassOutput + DNC_const;
    m_first_int2_stateVar = lowpassOutput + DNC_const;

    float outputSample_1 = lowpassOutput  * std::max(-(_signal[SVF_LBH_1]), 0.f);
    outputSample_1 += (bandpassOutput * (1.f - std::abs(_signal[SVF_LBH_1])));
    outputSample_1 += (highpassOutput * std::max(_signal[SVF_LBH_1], 0.f));


    //************************** 1st Stage Parabol Sat ***********************//
    m_first_sat_stateVar = std::clamp(bandpassOutput, -2.f, 2.f);
    m_first_sat_stateVar *= (1.f - std::abs(m_first_sat_stateVar) * 0.25f);


    //************************** 2nd Stage SV FILTER *************************//
    inputSample = (outputSample_1 * _signal[SVF_PAR_3])
                + (mixSample * _signal[SVF_PAR_4])
                + (m_second_sat_stateVar * 0.1f);

    omega = (_signal[SVF_F2_CUT] + fmVar * _signal[SVF_F2_FM]) * m_warpConst_2PI;
    omega = std::clamp(omega, 0.f, omegaMax);             /// initially 1.5f


//    attenuation = ((2.f + omega) * (2.f - omega) * tmpRes)
//            / (((tmpRes * omega) + (2.f - omega)) * 2.f);

    highpassOutput = inputSample - (m_second_int1_stateVar * attenuation + m_second_int2_stateVar);
    bandpassOutput = highpassOutput * omega + m_second_int1_stateVar;
    lowpassOutput  = bandpassOutput * omega + m_second_int2_stateVar;

    m_second_int1_stateVar = bandpassOutput + DNC_const;
    m_second_int2_stateVar = lowpassOutput + DNC_const;

    tmpVar  =  lowpassOutput  * std::max(-(_signal[SVF_LBH_2]), 0.f);
    tmpVar += (bandpassOutput * (1.f - std::abs(_signal[SVF_LBH_2])));
    tmpVar += (highpassOutput * std::max(_signal[SVF_LBH_2], 0.f));


    //************************* 2nd Stage Parabol Sat ************************//
    m_second_sat_stateVar = std::clamp(bandpassOutput, -2.f, 2.f);
    m_second_sat_stateVar *= (1.f - std::abs(m_second_sat_stateVar) * 0.25f);


    //****************************** Crossfades ******************************//
    m_out = (outputSample_1 * _signal[SVF_PAR_1]) + (tmpVar * _signal[SVF_PAR_2]);
}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_svfilter_ni::resetDSP()
{
    m_out = 0.f;

    m_first_int1_stateVar = 0.f;
    m_first_int2_stateVar = 0.f;
    m_second_int1_stateVar = 0.f;
    m_second_int2_stateVar = 0.f;

    m_first_sv_sample = 0.f;
    m_first_sat_stateVar = 0.f;
    m_second_sat_stateVar = 0.f;
}
