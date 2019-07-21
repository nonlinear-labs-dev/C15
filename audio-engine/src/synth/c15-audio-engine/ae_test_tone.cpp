#include "ae_test_tone.h"

void ae_test_tone::init(const float _samplerate)
{
    m_sample_increment = 1.f / _samplerate;
    m_phase = 0.f;
    m_signal = 0.f;
    m_focus = 0;
    a_frequency = test_tone_initial_freq;
    a_amplitude = test_tone_initial_gain;
    set_frequency(a_frequency);
    set_amplitude(a_amplitude);
    set_state(test_tone_initial_state);
}

void ae_test_tone::set_amplitude(const float _gain)
{
    m_amplitude = NlToolbox::Conversion::db2af(_gain);
}

void ae_test_tone::set_frequency(const float _frequency)
{
    m_frequency = _frequency;
}

void ae_test_tone::set_state(const uint32_t _state)
{
    m_state = _state;
    if(m_state == 0)
    {
        m_signal = 0.f;
    }
    else
    {
        set_frequency(a_frequency);
        set_amplitude(a_amplitude);
    }
}

void ae_test_tone::tick()
{
    // signal forming
    float phs = m_phase - 0.25f;
    phs -= NlToolbox::Conversion::float2int(phs);
    m_signal = m_amplitude * NlToolbox::Math::sinP3_noWrap(phs);
    // phase increment and wrapping
    m_phase += m_sample_increment * m_frequency;
    m_phase -= NlToolbox::Conversion::float2int(m_phase);
}
