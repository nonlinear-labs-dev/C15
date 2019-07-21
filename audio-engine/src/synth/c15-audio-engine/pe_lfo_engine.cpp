#include "pe_lfo_engine.h"

void stereo_lfo::tick()
{
    /* generate phase signal for left and right signal components */
    m_phase = phaseWrap(m_phase + m_increment);
    float r_phase = phaseWrap(m_phase + m_phaseOffset);
    /* generate left and right signals */
    m_left = m_phase * (8.f - (std::abs(m_phase) * 16.f));
    m_right = r_phase * (8.f - (std::abs(r_phase) * 16.f));
}

float stereo_lfo::phaseWrap(const float _phase)
{
    /* subtract rounded phase value from phase */
    return(_phase - static_cast<float>(NlToolbox::Conversion::float2int(_phase)));
}
