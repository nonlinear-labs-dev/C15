#pragma once

#include <stdint.h>

#include "pe_defines_config.h"
#include "nltoolbox.h"

struct ae_test_tone
{
    float m_sample_increment, m_phase, m_frequency, m_amplitude, m_signal;
    float a_frequency, a_amplitude;
    uint32_t m_state, m_focus;
    void init(const float _samplerate);
    void set_amplitude (const float _gain);
    void set_frequency(const float _frequency);
    void set_state(const uint32_t _state);
    void tick();
};
