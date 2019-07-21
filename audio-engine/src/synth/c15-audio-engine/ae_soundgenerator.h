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

class SignalStorage;

struct ae_soundgenerator
{
  ae_soundgenerator();  // Default Constructor

  FloatVector m_out_A, m_out_B;  // Generated Samples

  void init(float _samplerate);
  void generate(const FloatVector &_feedbackSample, SignalStorage &signals);
  void set(SignalStorage &signals, uint32_t _voiceID);
  void resetPhase(float _phase, uint32_t _voiceID);
  void resetDSP();

  //************************** Shared Variables *****************************//
  FloatVector m_sample_interval;
  FloatVector m_feedback_phase;
  FloatVector m_mute_state_A{ 1 };
  FloatVector m_mute_state_B{ 1 };

  //*********************** Oscillator A Variables **************************//
  FloatVector m_oscA_selfmix;
  FloatVector m_oscA_crossmix;

  FloatVector m_oscA_phase;
  FloatVector m_oscA_phase_stateVar;
  FloatVector m_oscA_phaseInc;

  Int32Vector m_OscA_randVal_int;
  FloatVector m_OscA_randVal_float;

  //*********************** Oscillator B Variables **************************//
  FloatVector m_oscB_selfmix;
  FloatVector m_oscB_crossmix;

  FloatVector m_oscB_phase;
  FloatVector m_oscB_phase_stateVar;
  FloatVector m_oscB_phaseInc;

  Int32Vector m_OscB_randVal_int;
  FloatVector m_OscB_randVal_float;

  //***************************** Chirp Filter ******************************//
  float m_warpConst_PI;
  FloatVector m_chiA_stateVar, m_chiB_stateVar;

  FloatVector m_chiA_omega, m_chiA_a0, m_chiA_a1;
  FloatVector m_chiB_omega, m_chiB_a0, m_chiB_a1;
};
