/******************************************************************************/
/** @file           ae_combfilter.h
    @date           2018-05-10
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter
    @todo
*******************************************************************************/

#pragma once

#include <vector>
#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"

#include "ParallelData.h"

class ParameterStorage;

using namespace NlToolbox::Constants;

#define COMB_BUFFER_SIZE 8192

struct ae_combfilter
{
  ae_combfilter();  // Default Contructor

  FloatVector m_out;  // Generated Sample
  FloatVector m_decayStateVar;

  float m_sampleInterval;
  float m_warpConst_PI;
  float m_warpConst_2PI;
  float m_freqClip_2;
  float m_freqClip_4;
  float m_freqClip_24576;

  void init(float _samplerate, uint32_t _upsampleFactor);
  void apply(const FloatVector &_sampleA, const FloatVector &_sampleB, ParameterStorage &params);
  void set(uint32_t voice, ParameterStorage &params, float _samplerate);
  void setDelaySmoother(uint32_t voice);
  void resetDSP();

  //**************************** Highpass Filter ****************************//
  FloatVector m_hpCoeff_b0, m_hpCoeff_b1, m_hpCoeff_a1;
  FloatVector m_hpInStateVar, m_hpOutStateVar;

  //***************************** Lowpass Filter ****************************//
  FloatVector m_lpCoeff;
  FloatVector m_lpStateVar;

  //***************************** Allpass Filter ****************************//
  FloatVector m_apCoeff_1, m_apCoeff_2;
  FloatVector m_apStateVar_1;  // Allpass State Variables
  FloatVector m_apStateVar_2;
  FloatVector m_apStateVar_3;
  FloatVector m_apStateVar_4;

  //****************************** Delay/ Decay *****************************//
  uint32_t m_buffer_indx;
  uint32_t m_buffer_sz_m1;
  std::vector<FloatVector> m_buffer;

  FloatVector m_delaySamples;
  float m_delayFreqClip;
  FloatVector m_delayConst;
  FloatVector m_delayStateVar;

  FloatVector m_decayGain;
};
