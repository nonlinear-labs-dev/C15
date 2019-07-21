/******************************************************************************/
/** @file           ae_echo.h
    @date           2018-07-26
    @version        1.0
    @author         Anton Schmied
    @brief          Echo
    @todo
*******************************************************************************/

#pragma once

#include "dsp_defines_signallabels.h"
#include "nltoolbox.h"

class SignalStorage;
using namespace NlToolbox::Constants;

#include <vector>

#define ECHO_BUFFER_SIZE 131072

struct ae_echo
{
  ae_echo();  // Default Constructor

  float m_out_L, m_out_R;

  void init(float _samplerate, uint32_t _upsampleFactor);
  void apply(float _rawSample_L, float _rawSample_R, SignalStorage &signals);
  void set(SignalStorage &signals);
  void resetDSP();

  float m_warpConst_PI;
  float m_freqClip_min, m_freqClip_max;

  //*************************** 1 pole Highpass ****************************//
  float m_hp_b0, m_hp_b1, m_hp_a1;

  float m_hp_stateVar_L1, m_hp_stateVar_L2;
  float m_hp_stateVar_R1, m_hp_stateVar_R2;

  //**************************** 1 pole Lowpass ****************************//
  float m_lp_b0, m_lp_b1, m_lp_a1;

  float m_lp_stateVar_L1, m_lp_stateVar_L2;
  float m_lp_stateVar_R1, m_lp_stateVar_R2;

  //****************************** 2Hz Lowpass *****************************//
  float m_lp2hz_b0;
  float m_lp2hz_stateVar_L, m_lp2hz_stateVar_R;

  //***************************** Delay Buffer *****************************//
  float m_stateVar_L, m_stateVar_R;

  int32_t m_buffer_indx;
  int32_t m_buffer_sz_m1;
  std::vector<float> m_buffer_L;
  std::vector<float> m_buffer_R;
};
