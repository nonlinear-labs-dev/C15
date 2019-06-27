/******************************************************************************/
/** @file           ae_svfilter_ni.h
    @date           2019-06-26
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter as used in the primary module
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"

using namespace NlToolbox::Constants;


struct ae_svfilter_ni
{
    ae_svfilter_ni();      // Default Constructor

    float m_out;
    float m_warpConst_2PI;

    void init(float _samplerate);
    void apply(float _sampleA, float _sampleB, float _sampleComb, float *_signal);
    void resetDSP();

    //**************************** State Variables ****************************//
    float m_first_int1_stateVar, m_first_int2_stateVar;
    float m_second_int1_stateVar, m_second_int2_stateVar;

    float m_first_sv_sample;
    float m_first_sat_stateVar, m_second_sat_stateVar;
};
