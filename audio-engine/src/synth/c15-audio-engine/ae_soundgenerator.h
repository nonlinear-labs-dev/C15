/******************************************************************************/
/** @file           ae_soundgenerator.h
    @date           2018-03-22
    @version        1.0
    @author         Anton Schmied
    @brief          Soundgenerator which contains both Oscillators and Shapers
                    receiving the rendered parameter signals from the
                    param engine
    @todo
*******************************************************************************/

#pragma once

#include <cmath>
#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"
#include "pe_defines_config.h"
#include "ParallelData.h"

class ParameterStorage;

struct ae_soundgenerator
{
  ae_soundgenerator();  // Default Constructor

  float m_out_A, m_out_B;  // Generated Samples

  void init(float _samplerate, uint32_t _vn);
  void generate(float _feedbackSample, ParameterStorage &params);
  void set(ParameterStorage &params);
  void resetPhase(float _phase);
  void resetDSP();

  //************************** Shared Variables *****************************//
  float m_sample_interval;
  float m_feedback_phase;
  const float m_mute_state[2] = { 1.f, 0.f };

  //*********************** Oscillator A Variables **************************//
  float m_oscA_selfmix;
  float m_oscA_crossmix;

  float m_oscA_phase;
  float m_oscA_phase_stateVar;
  float m_oscA_phaseInc;

  int32_t m_OscA_randVal_int;
  float m_OscA_randVal_float;
  uint32_t m_OscA_mute;

  //*********************** Oscillator B Variables **************************//
  float m_oscB_selfmix;
  float m_oscB_crossmix;

  float m_oscB_phase;
  float m_oscB_phase_stateVar;
  float m_oscB_phaseInc;

  int32_t m_OscB_randVal_int;
  float m_OscB_randVal_float;
  uint32_t m_OscB_mute;

  //***************************** Chirp Filter ******************************//
  float m_warpConst_PI;
  float m_chiA_stateVar, m_chiB_stateVar;

  float m_chiA_omega, m_chiA_a0, m_chiA_a1;
  float m_chiB_omega, m_chiB_a0, m_chiB_a1;
};
