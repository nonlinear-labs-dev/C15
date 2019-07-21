/******************************************************************************/
/** @file           ae_svfilter.h
    @date           2018-07-05
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter with no FIR Filter Version and FM+F clipping
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"
#include "ParallelData.h"

class SignalStorage;
using namespace NlToolbox::Constants;

#include "pe_defines_config.h"  /// for testing the clipping limits

struct ae_svfilter
{
  ae_svfilter();  // Default Constructor

  FloatVector m_out;
  float m_warpConst_2PI;

  void init(float _samplerate);
  void apply(const FloatVector &_sampleA, const FloatVector &_sampleB, const FloatVector &_sampleComb,
             SignalStorage &signals);
  void resetDSP();

  //**************************** State Variables ****************************//
  FloatVector m_first_int1_stateVar, m_first_int2_stateVar;
  FloatVector m_second_int1_stateVar, m_second_int2_stateVar;

  FloatVector m_first_sv_sample;
  FloatVector m_first_sat_stateVar, m_second_sat_stateVar;
};
