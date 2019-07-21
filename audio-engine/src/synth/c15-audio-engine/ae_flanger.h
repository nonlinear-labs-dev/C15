/******************************************************************************/
/** @file           ae_flanger.h
    @date           2018-07-27
    @version        1.0
    @author         Anton Schmied
    @brief          Echo
    @todo
*******************************************************************************/

#pragma once

#include "dsp_defines_signallabels.h"
#include "nltoolbox.h"
#include <vector>

class SignalStorage;
using namespace NlToolbox::Constants;

#define FLANGER_BUFFER_SIZE 8192

struct ae_flanger
{
  ae_flanger();  // Default Constructor

  float m_out_L, m_out_R;

  void init(float _samplerate, uint32_t _upsampleFactor);
  void set_slow(SignalStorage &signals);
  void set_fast(SignalStorage &signals);
  void apply(float _rawSample_L, float _rawSample_R, SignalStorage &signals);
  void resetDSP();

  float m_warpConst_PI, m_warpConst_2PI;
  float m_freqClip_min;
  float m_freqClip_max;

  //**************************** 1 pole Lowpass ****************************//
  float m_lp_b0, m_lp_b1, m_lp_a1;

  float m_lp_stateVar_L1, m_lp_stateVar_L2;
  float m_lp_stateVar_R1, m_lp_stateVar_R2;

  //*************************** 1 pole Highpass ****************************//
  float m_hp_b0, m_hp_b1, m_hp_a1;

  float m_hp_stateVar_L1, m_hp_stateVar_L2;
  float m_hp_stateVar_R1, m_hp_stateVar_R2;

  //****************************** 2Hz Lowpass *****************************//
  float m_lp2hz_b0;
  float m_lp2hz_stateVar_TL, m_lp2hz_stateVar_TR, m_lp2hz_stateVar_D;

  //**************************** 4 Pole Allpass ****************************//
  float m_ap_b0_L, m_ap_b1_L;
  float m_ap_b0_R, m_ap_b1_R;

  float m_ap_StateVar_L1, m_ap_StateVar_R1;
  float m_ap_StateVar_L2, m_ap_StateVar_R2;
  float m_ap_StateVar_L3, m_ap_StateVar_R3;
  float m_ap_StateVar_L4, m_ap_StateVar_R4;
  float m_ap_StateVar_L5, m_ap_StateVar_R5;
  float m_ap_StateVar_L6, m_ap_StateVar_R6;
  float m_ap_StateVar_L7, m_ap_StateVar_R7;
  float m_ap_StateVar_L8, m_ap_StateVar_R8;

  //***************************** Delay Buffer *****************************//
  float m_stateVar_L, m_stateVar_R;

  int32_t m_buffer_indx;
  int32_t m_buffer_sz_m1;
  std::vector<float> m_buffer_L;
  std::vector<float> m_buffer_R;
};
