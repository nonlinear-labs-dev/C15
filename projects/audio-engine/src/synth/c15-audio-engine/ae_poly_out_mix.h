#pragma once

/******************************************************************************/
/** @file       ae_poly_out_mix.h
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      output mixer
    @todo
*******************************************************************************/

#include "nltoolbox.h"
#include <vector>
#include "ae_info.h"

namespace Engine
{
  class PolyOutputMixer
  {
   public:
    float m_out_l = 0.0f, m_out_r = 0.0f;
    PolyOutputMixer();
    void init(const float _samplerate, const uint32_t _numOfVoices);
    void combine(PolySignals &_signals, const PolyValue &_voiceLevel, const PolyValue &_sampleA,
                 const PolyValue &_sampleB, const PolyValue &_sampleComb, const PolyValue &_sampleSVFilter);
    void filter_level(PolySignals &_signals);
    void resetDSP();

   private:
    PolyValue m_hp30hz_stateVar_L = {}, m_hp30hz_stateVar_R = {};
    float m_hp30hz_b0 = 0.0f, m_hp_b0 = 0.0f, m_hp_b1 = 0.0f, m_hp_a1 = 0.0f, m_hp_stateVar_L1 = 0.0f,
          m_hp_stateVar_R1 = 0.0f, m_hp_stateVar_L2 = 0.0f, m_hp_stateVar_R2 = 0.0f;
  };
}  // namespace Engine
