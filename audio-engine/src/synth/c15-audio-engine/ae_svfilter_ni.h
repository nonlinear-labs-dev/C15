/******************************************************************************/
/** @file           ae_svfilter_ni.h
    @date           2019-07-03
    @version        1.0
    @author         Anton Schmied, Matthias Seeber
    @brief          State Variable Filter as used in the primary module
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"
#include "ParallelData.h"

class SignalStorage;
using namespace NlToolbox::Constants;

struct ae_svfilter_ni
{
    ae_svfilter_ni();       // Default Constructor

    FloatVector m_out;
    float m_warpConst_2PI;

    void init(float _samplerate);
    void apply(const FloatVector &_sampleA, const FloatVector &_sampleB,
               const FloatVector &_sampleComb, SignalStorage &signals);
    void resetDSP();

    //**************************** State Variables ****************************//
    FloatVector m_first_int1_stateVar, m_first_int2_stateVar;
    FloatVector m_second_int1_stateVar, m_second_int2_stateVar;

    FloatVector m_first_sat_stateVar, m_second_sat_stateVar;
};
