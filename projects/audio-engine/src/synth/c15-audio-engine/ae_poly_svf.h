#pragma once

/******************************************************************************/
/** @file       ae_poly_svf.h
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      state variable filter
    @todo
*******************************************************************************/

#include "nltoolbox.h"
#include "ae_info.h"

namespace Engine
{
  class PolyStateVariableFilter
  {
   public:
    PolyValue m_out = {};
    PolyStateVariableFilter();
    void init(const float _samplerate);
    void apply(PolySignals &_signals, const PolyValue &_sampleA, const PolyValue &_sampleB,
               const PolyValue &_sampleComb);
    void resetDSP();

   private:
    PolyValue m_first_int1_stateVar = {}, m_first_int2_stateVar = {}, m_second_int1_stateVar = {},
              m_second_int2_stateVar = {}, m_first_sat_stateVar = {}, m_second_sat_stateVar = {};
    float m_warpConst_2PI = 0.0f;
  };
}  // namespace Engine
