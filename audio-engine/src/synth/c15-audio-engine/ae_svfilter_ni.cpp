/******************************************************************************/
/** @file           ae_svfilter_ni.cpp
    @date           2019-07-03
    @version        1.0
    @author         Anton Schmied, Matthias Seeber
    @brief          State Variable Filter as used in the primary module
    @todo
*******************************************************************************/

#include "ae_svfilter_ni.h"
#include "SignalStorage.h"

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

    m_warpConst_2PI = twopi / _samplerate;

    m_first_int1_stateVar = 0.0f;
    m_first_int2_stateVar = 0.0f;
    m_second_int1_stateVar = 0.0f;
    m_second_int2_stateVar = 0.0f;

    m_first_sat_stateVar = 0.0f;
    m_second_sat_stateVar = 0.0f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_svfilter_ni::apply(const FloatVector &_sampleA, const FloatVector &_sampleB,
                           const FloatVector &_sampleComb, SignalStorage &_signals)
{
    //*************************** Damp, Fmax (from Res) **********************//
    auto damp = _signals.get<Signals::SVF_RES_DAMP>();
    auto fmax = _signals.get<Signals::SVF_RES_FMAX>();

    //******************************** Sample Mix ****************************//
    auto tmpVar = _signals.get<Signals::SVF_AB>();
    auto mixSample = (_sampleB * (1.0f - tmpVar)) + (_sampleA * tmpVar);
    tmpVar = _signals.get<Signals::SVF_CMIX>();
    mixSample = (mixSample * (1.0f - std::abs(tmpVar))) + (_sampleComb * tmpVar);

    //************************** Frequency Modulation ************************//
    tmpVar = _signals.get<Signals::SVF_FMAB>();
    auto fmVar = (_sampleA * (1.0f - tmpVar)) + (_sampleB * tmpVar);

    //************************** 1st Stage SV FILTER *************************//
    auto inputSample = mixSample + (m_first_sat_stateVar * 0.10f);

    auto omega = (_signals.get<Signals::SVF_F1_CUT>() + (fmVar * _signals.get<Signals::SVF_F1_FM>())) * m_warpConst_2PI;
    /* Here, we will perform clipping depending on the resonance! */
    omega = std::clamp(omega, 0.f, fmax);

    auto highpassOutput = inputSample - ((m_first_int1_stateVar * damp) + m_first_int2_stateVar);
    auto bandpassOutput = (highpassOutput * omega) + m_first_int1_stateVar;
    auto lowpassOutput = (bandpassOutput * omega) + m_first_int2_stateVar;

    m_first_int1_stateVar = bandpassOutput;
    m_first_int2_stateVar = lowpassOutput + DNC_const;
    /* strictly following ni template here (only LP denormed) - however, denorm value difference remains: 1e-12 (ni) vs 1e-18 */

    tmpVar = _signals.get<Signals::SVF_LBH_1>();
    auto outputSample_1 = lowpassOutput * std::max(-tmpVar, 0.0f);
    outputSample_1 += bandpassOutput * (1.0f - std::abs(tmpVar));
    outputSample_1 += highpassOutput * std::max(tmpVar, 0.0f);

    //************************** 1st Stage Parabol Sat ***********************//
    m_first_sat_stateVar = std::clamp(bandpassOutput, -2.0f, 2.0f);
    m_first_sat_stateVar *= 1.0f - (std::abs(m_first_sat_stateVar) * 0.250f);

    //************************** 2nd Stage SV FILTER *************************//
    inputSample = (outputSample_1 * _signals.get<Signals::SVF_PAR_3>())
                + (mixSample * _signals.get<Signals::SVF_PAR_4>())
                + (m_second_sat_stateVar * 0.10f);

    omega = (_signals.get<Signals::SVF_F2_CUT>() + (fmVar * _signals.get<Signals::SVF_F2_FM>())) * m_warpConst_2PI;
    omega = std::clamp(omega, 0.0f, fmax);

    highpassOutput = inputSample - ((m_second_int1_stateVar * damp) + m_second_int2_stateVar);
    bandpassOutput = (highpassOutput * omega) + m_second_int1_stateVar;
    lowpassOutput = (bandpassOutput * omega) + m_second_int2_stateVar;

    m_second_int1_stateVar = bandpassOutput;
    m_second_int2_stateVar = lowpassOutput + DNC_const;
    /* strictly following ni template here (only LP denormed) - however, denorm value difference remains: 1e-12 (ni) vs 1e-18 */

    tmpVar = _signals.get<Signals::SVF_LBH_2>();
    auto outputSample_2 = lowpassOutput * std::max(-tmpVar, 0.0f);
    outputSample_2 += bandpassOutput * (1.0f - std::abs(tmpVar));
    outputSample_2 += highpassOutput * std::max(tmpVar, 0.0f);

    //************************* 2nd Stage Parabol Sat ************************//
    m_second_sat_stateVar = std::clamp(bandpassOutput, -2.0f, 2.0f);
    m_second_sat_stateVar *= 1.0f - (std::abs(m_second_sat_stateVar) * 0.250f);

    //****************************** Crossfades ******************************//
    m_out = (outputSample_1 * _signals.get<Signals::SVF_PAR_1>()) + (outputSample_2 * _signals.get<Signals::SVF_PAR_2>());
}

void ae_svfilter_ni::resetDSP()
{
    m_out = 0.0f;

    m_first_int1_stateVar = 0.0f;
    m_first_int2_stateVar = 0.0f;
    m_second_int1_stateVar = 0.0f;
    m_second_int2_stateVar = 0.0f;

    m_first_sat_stateVar = 0.0f;
    m_second_sat_stateVar = 0.0f;
}
