/******************************************************************************/
/** @file           ae_cabinet.h
    @date           2018-07-18
    @version        1.0
    @author         Anton Schmied
    @brief          Cabinet
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"

class SignalStorage;

using namespace NlToolbox::Constants;

struct ae_cabinet
{
  ae_cabinet();  // Default Constructor

  float m_out_L, m_out_R;

  float m_warpConst_2PI;
  float m_freqClip_max, m_freqClip_min;

  void init(float _samplerate);
  void apply(float _rawSample_L, float _rawSample_R, SignalStorage &signals);
  void set(SignalStorage &signals);
  void resetDSP();

  //**************************** Biquad Highpass ****************************//
  float m_hp_b0, m_hp_b1;
  float m_hp_a1, m_hp_a2;

  float m_hp_stateVar_L1, m_hp_stateVar_R1;
  float m_hp_stateVar_L2, m_hp_stateVar_R2;
  float m_hp_stateVar_L3, m_hp_stateVar_R3;
  float m_hp_stateVar_L4, m_hp_stateVar_R4;

  //***************************** Biquad Lowpass ****************************//
  float m_lp1_b0, m_lp1_b1;
  float m_lp1_a1, m_lp1_a2;

  float m_lp1_stateVar_L1, m_lp1_stateVar_R1;
  float m_lp1_stateVar_L2, m_lp1_stateVar_R2;
  float m_lp1_stateVar_L3, m_lp1_stateVar_R3;
  float m_lp1_stateVar_L4, m_lp1_stateVar_R4;

  float m_lp2_b0, m_lp2_b1;
  float m_lp2_a1, m_lp2_a2;

  float m_lp2_stateVar_L1, m_lp2_stateVar_R1;
  float m_lp2_stateVar_L2, m_lp2_stateVar_R2;
  float m_lp2_stateVar_L3, m_lp2_stateVar_R3;
  float m_lp2_stateVar_L4, m_lp2_stateVar_R4;

  //**************************** Tilt Lowshelves ****************************//
  float m_tiltOmegaSin, m_tiltOmegaCos;

  float m_ls1_b0, m_ls1_b1, m_ls1_b2;
  float m_ls1_a1, m_ls1_a2;

  float m_ls1_stateVar_L1, m_ls1_stateVar_R1;
  float m_ls1_StateVar_L2, m_ls1_stateVar_R2;
  float m_ls1_StateVar_L3, m_ls1_stateVar_R3;
  float m_ls1_StateVar_L4, m_ls1_stateVar_R4;

  float m_ls2_b0, m_ls2_b1, m_ls2_b2;
  float m_ls2_a1, m_ls2_a2;

  float m_ls2_stateVar_L1, m_ls2_stateVar_R1;
  float m_ls2_StateVar_L2, m_ls2_stateVar_R2;
  float m_ls2_StateVar_L3, m_ls2_stateVar_R3;
  float m_ls2_StateVar_L4, m_ls2_stateVar_R4;

  //***************************** 30 Hz Highpass ****************************//
  float m_hp30_b0;

  float m_hp30_stateVar_L, m_hp30_stateVar_R;
};
