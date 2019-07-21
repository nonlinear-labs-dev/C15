/******************************************************************************/
/** @file		pe_lfo_engine.h
    @date       2018-07-27
    @version    1.0
    @author     Matthias Seeber
    @brief		flanger lfo generator
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"

struct stereo_lfo
{
    /* local variables */
    float m_phase = 0.f;                    // current lfo phase
    float m_increment = 0.f;                // phase increment (depends on frequency)
    float m_phaseOffset = 0.f;              // phase offset (right to left)

    /* lfo signals */
    float m_left = 0.f;                     // left lfo signal
    float m_right = 0.f;                    // right lfo signal

    /* provided functions */
    void tick();                            // rendering
    float phaseWrap(const float _phase);    // (internal) phase wrapping
};
