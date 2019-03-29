/******************************************************************************/
/** @file           ae_outputmixer.h
    @date           2018-05-09
    @version        1.0
    @author         Anton Schmied
    @brief          Outputmixer
    @todo           Filter auflösen!
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"
#include "ParallelData.h"
#include <vector>

class ParameterStorage;
using namespace NlToolbox::Constants;

struct ae_outputmixer
{
  ae_outputmixer();  // Default Constructor

  float m_out_L, m_out_R;

  float m_hp30hz_b0;

  FloatVector m_hp30hz_stateVar_L;
  FloatVector m_hp30hz_stateVar_R;

  void init(float _samplerate, uint32_t _numOfVoices);
  void combine(float _sampleA, float _sampleB, float _sampleComb, float _sampleSVFilter, ParameterStorage &params,
               uint32_t _voiceID);

  void combine(const FloatVector &_sampleA, const FloatVector &_sampleB, const FloatVector &_sampleComb,
               const FloatVector &_sampleSVFilter, ParameterStorage &params);

  void filter_level(ParameterStorage &params);
  void resetDSP();

  //*************************** Highpass Filters ****************************//
  float m_hp_b0, m_hp_b1, m_hp_a1;
  float m_hp_stateVar_L1, m_hp_stateVar_R1;
  float m_hp_stateVar_L2, m_hp_stateVar_R2;
};
