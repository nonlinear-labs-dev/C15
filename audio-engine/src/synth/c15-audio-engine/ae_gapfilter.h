/******************************************************************************/
/** @file           ae_gapfilter.h
    @date           2018-07-25
    @version        1.0
    @author         Anton Schmied
    @brief          Gap Filter
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"

class SignalStorage;

struct ae_gapfilter
{
  ae_gapfilter();  // Default Constructor

  float m_out_L, m_out_R;

  void init(float _samplerate);
  void apply(float _rawSample_L, float _rawSample_R, SignalStorage &signals);
  void set(SignalStorage &signals);
  void resetDSP();

  float m_warpConst_2PI;
  float m_freqClip_max, m_freqClip_min;
  float m_freqScale_max, m_freqScale_min;

  //************************** Biquad Highpass L/R **************************//
  float m_hp_l1_b0, m_hp_l1_b1, m_hp_l2_b0, m_hp_l2_b1;
  float m_hp_l1_a1, m_hp_l1_a2, m_hp_l2_a1, m_hp_l2_a2;

  float m_hp_l1_stateVar_1, m_hp_l2_stateVar_1;
  float m_hp_l1_stateVar_2, m_hp_l2_stateVar_2;
  float m_hp_l1_stateVar_3, m_hp_l2_stateVar_3;
  float m_hp_l1_stateVar_4, m_hp_l2_stateVar_4;

  float m_hp_r1_b0, m_hp_r1_b1, m_hp_r2_b0, m_hp_r2_b1;
  float m_hp_r1_a1, m_hp_r1_a2, m_hp_r2_a1, m_hp_r2_a2;

  float m_hp_r1_stateVar_1, m_hp_r2_stateVar_1;
  float m_hp_r1_stateVar_2, m_hp_r2_stateVar_2;
  float m_hp_r1_stateVar_3, m_hp_r2_stateVar_3;
  float m_hp_r1_stateVar_4, m_hp_r2_stateVar_4;

  //*************************** Biquad Lowpass L/R **************************//
  float m_lp_l1_b0, m_lp_l1_b1, m_lp_l2_b0, m_lp_l2_b1;
  float m_lp_l1_a1, m_lp_l1_a2, m_lp_l2_a1, m_lp_l2_a2;

  float m_lp_l1_stateVar_1, m_lp_l2_stateVar_1;
  float m_lp_l1_stateVar_2, m_lp_l2_stateVar_2;
  float m_lp_l1_stateVar_3, m_lp_l2_stateVar_3;
  float m_lp_l1_stateVar_4, m_lp_l2_stateVar_4;

  float m_lp_r1_b0, m_lp_r1_b1, m_lp_r2_b0, m_lp_r2_b1;
  float m_lp_r1_a1, m_lp_r1_a2, m_lp_r2_a1, m_lp_r2_a2;

  float m_lp_r1_stateVar_1, m_lp_r2_stateVar_1;
  float m_lp_r1_stateVar_2, m_lp_r2_stateVar_2;
  float m_lp_r1_stateVar_3, m_lp_r2_stateVar_3;
  float m_lp_r1_stateVar_4, m_lp_r2_stateVar_4;
};
